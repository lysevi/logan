#include "log.h"
#include "logviewer.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QPair>
#include <QRegExp>
//#include <QtConcurrent>
#include <QApplication>
#include <QScrollBar>
#include <QTextCodec>

const char LOG_ENDL = '\n';
const int PROGR_STEP = 300;

bool allLinePos(QProgressDialog *progress_dlg, const QByteArray &bts,
                LinePositionList &result) {
  if (progress_dlg != nullptr) {
    progress_dlg->setRange(0, bts.size());
  }
  int progr = 0;
  int count = 0;
  auto size = bts.size();
  for (int i = 0; i < size; ++i) {
    if (bts[i] == LOG_ENDL) {
      count++;
    }
    if (progress_dlg != nullptr && progr % PROGR_STEP == 0) {
      progress_dlg->setValue(progr);
      QApplication::processEvents();
    }
    if (progress_dlg != nullptr && progress_dlg->wasCanceled()) {
      return false;
    }
    progr++;
  }
  result.resize(count);
  int start = 0;
  int index = 0;
  for (int i = 0; i < size; ++i) {
    if (bts[i] == LOG_ENDL) {
      LinePosition lp;
      lp.first = start;
      lp.second = i;
      result[index++] = lp;
      start = i + 1;
    }
    if (progress_dlg != nullptr && progr % 1000 == 0) {
      progress_dlg->setValue(progr);
      QApplication::processEvents();
    }
    if (progress_dlg != nullptr && progress_dlg->wasCanceled()) {
      return false;
    }
    progr++;
  }
  return true;
}

Log::Log(const QFileInfo &fileInfo, const QString &filename,
         const HighlightPatterns *global_highlight, const QString &default_encoding,
         QObject *parent)
    : QAbstractItemModel(parent) {

  m_default_encoding = default_encoding;
  m_fileInfo = fileInfo;
  m_lastModifed = m_fileInfo.lastModified();
  m_name = m_fileInfo.fileName();
  m_fname = filename;
  m_global_highlight = global_highlight;
  m_load_complete = false;
  m_codec = QTextCodec::codecForName(m_default_encoding.toStdString().c_str());

  if (m_codec == nullptr) {
    throw std::logic_error("m_codec==nullptr");
  }
}

Log *Log::openFile(const QString &fname, const HighlightPatterns *global_highlight,
                   const QString &default_encoding, QObject *parent) {
  qDebug() << "openFile" << fname;
  QFileInfo fileInfo(fname);
  auto ll = new Log(fileInfo, fname, global_highlight, default_encoding, parent);

  if (!ll->loadFile()) {
    qDebug() << "not loaded " << ll->m_name;
    delete ll;
    return nullptr;
  }
  qDebug() << "loaded " << ll->m_name << " lines:" << ll->m_lines.size();
  return ll;
}

bool Log::loadFile() {
  bool result = false;
  qDebug() << "loadFile " << m_fname;
  auto curDT = QDateTime::currentDateTimeUtc();
  QFile inputFile(m_fname);
  if (inputFile.open(QIODevice::ReadOnly)) {
    QProgressDialog progress_dlg(MainWindow::instance);
    progress_dlg.setWindowTitle(tr("File loading: ") + m_fname);
    progress_dlg.setWindowModality(Qt::WindowModal);
    progress_dlg.setModal(true);
    progress_dlg.setAutoClose(true);

    m_bts = inputFile.readAll();
    result = allLinePos(&progress_dlg, m_bts, m_lines);
    if (result) {
      m_cache.resize(m_lines.size());
    }
    inputFile.close();
  } else {
    throw std::logic_error("file not exists!");
  }
  qDebug() << "elapsed time:" << curDT.secsTo(QDateTime::currentDateTimeUtc());
  m_load_complete = true;
  return result;
}

QString Log::name() const {
  return m_name;
}

QString Log::filename() const {
  return m_fname;
}

void Log::update() {
  qDebug() << "update " << m_name << "=>" << m_fname;
  std::lock_guard<std::mutex> lg(_locker);
  QFileInfo fileInfo(m_fname);

  m_lastModifed = fileInfo.lastModified();

  qDebug() << "update " << m_fname;
  auto curDT = QDateTime::currentDateTimeUtc();
  QFile inputFile(m_fname);
  if (inputFile.open(QIODevice::ReadOnly)) {
    auto bts = inputFile.readAll();
    LinePositionList lines;
    allLinePos(nullptr, bts, lines);
    auto diff = lines.size() - m_lines.size();
    if (diff > 0) {
      qDebug() << "diff " << diff;
      // auto old_size = m_lines.size();
      m_load_complete = false;

      m_bts = std::move(bts);

      m_lines = std::move(lines);
      m_cache.clear();
      m_cache.resize(m_lines.size());

      m_load_complete = true;

      emit countChanged(m_lines.size());
      emit linesChanged();

      if (_fltr != nullptr) {
        resetFilter(_fltr);
      } else {
        beginResetModel();
        endResetModel();
        //        beginInsertRows(createIndex(0, 0, nullptr), old_size - 1,
        //        old_size +
        //        diff);
        //        endInsertRows();
      }
      // emit m_lv_object->scrollToBottom();
    }

    inputFile.close();
  } else {
    throw std::logic_error("file not exists!");
  }

  qDebug() << "update elapsed time:" << curDT.secsTo(QDateTime::currentDateTimeUtc());
}

int Log::rowCount(const QModelIndex &parent) const {
  // qDebug() << "Log::rowCount";
  std::lock_guard<std::mutex> lg(_locker);
  Q_UNUSED(parent);
  if (!m_load_complete) {
    return 0;
  }
  if (_fltr != nullptr) {
    return m_fltr_cache.size();
  }
  return m_lines.size();
  // return m_cache.size();
}

QVariant Log::data(const QModelIndex &index, int role) const {

  const QString emptyString = "EmptyString";
  if (!m_load_complete) {
    return emptyString;
  }

  if (index.row() < 0 || index.row() >= int(m_lines.size()))
    return QVariant();
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    // qDebug() << "Log::data";
    std::lock_guard<std::mutex> lg(_locker);
    if (_fltr != nullptr) {
      return *m_fltr_cache[index.row()].Value;
    }
    if (m_cache[index.row()].Value != nullptr) {
      return *m_cache[index.row()].Value;
    } else {
      CachedString cs;
      cs.originValue = makeRawString(index.row());
      cs.index = index.row();
      rawStringToValue(cs.originValue);
      cs.Value = cs.originValue;
      m_cache[index.row()] = cs;
      return *cs.Value;
    }
  }
  return QVariant();
}

QString Log::plainText(const QModelIndex &index) const {
  if (index.row() < 0 || index.row() >= int(m_lines.size()))
    return QString("error");
  if (_fltr == nullptr) {
    return *makeRawString(index.row());
  } else {
    return *makeRawString(m_fltr_cache[index.row()].index);
  }
}

bool Log::heighlightStr(QString *str, const HighlightPattern &pattern) {
  if (pattern.pattern.size() == 0) {
    return false;
  }
  bool result = false;

  QRegExp re(pattern.pattern);
  if (re.indexIn(*str) != -1) {
    auto ct = re.capturedTexts();
    for (auto &&captured_str : ct) {
      str->replace(re,
                   "<font color=\"" + pattern.rgb.toUpper() + "\"><b>" + captured_str +
                       "</b></font>");
    }
    result = true;
  }

  return result;
}

void Log::setListVoxObject(LogViewer *object) {
  m_lv_object = object;
}

QPair<int, QString> Log::findFrom(const QString &pattern, int index,
                                  SearchDirection direction) {
  qDebug() << "Log::findFrom";
  std::lock_guard<std::mutex> lg(_locker);
  if (index == m_cache.size() || index < 0) {
    return QPair<int, QString>(index, QString());
  }

  QRegExp re(pattern.toUpper());

  int i = direction == SearchDirection::Down ? index + 1 : index - 1;

  while (true) {
    QString str = plainText(createIndex(int(i), 0, nullptr)).toUpper();
    if (re.indexIn(str) != -1) {
      return QPair<int, QString>(i, str);
    }
    if (direction == SearchDirection::Down) {
      i++;
      if (i == m_cache.size()) {
        break;
      }
    } else {
      i--;
      if (i < 0) {
        break;
      }
    }
  }
  return QPair<int, QString>(index, "");
}

void Log::resetFilter(const Filter_Ptr &fltr) {
  qDebug() << "Log::resetFilter";
  // std::lock_guard<std::mutex> lg(_locker);
  setFilter_impl(fltr);
}

void Log::setFilter(const Filter_Ptr &fltr) {
  qDebug() << "Log::setFilter";
  // std::lock_guard<std::mutex> lg(_locker);
  setFilter_impl(fltr);
}

void Log::setFilter_impl(const Filter_Ptr &fltr) {
  QProgressDialog progress_dlg(MainWindow::instance);
  progress_dlg.setWindowTitle(tr("Filter application."));
  progress_dlg.setWindowModality(Qt::WindowModal);
  progress_dlg.setModal(true);
  progress_dlg.setRange(0, linesCount());
  progress_dlg.setAutoClose(true);

  int count = 0;
  QVector<CachedString> new_fltr_cache(m_lines.size());

  for (size_t i = 0; i < m_lines.size(); ++i) {
    auto qs = makeRawString(i);
    if (fltr->inFilter(*qs)) {
      rawStringToValue(qs);
      CachedString cs;
      cs.index = i;
      cs.originValue = qs;
      cs.Value = cs.originValue;
      new_fltr_cache[count] = cs;
      count++;
    }

    if (i % PROGR_STEP == 0) {
      qDebug() << "i=" << i;
      progress_dlg.setValue(int(i));
      QApplication::processEvents();
    }
    if (progress_dlg.wasCanceled()) {
      return;
    }
  }
  beginResetModel();
  std::lock_guard<std::mutex> lg(_locker);
  new_fltr_cache.resize(count);
  _fltr = fltr;
  m_fltr_cache = std::move(new_fltr_cache);
  endResetModel();
}

void Log::clearFilter() {
  qDebug() << "Log::clearFilter";
  if (_fltr != nullptr) {
    beginResetModel();
    _fltr = nullptr;
    m_fltr_cache.resize(1);
    endResetModel();
  }
}

std::shared_ptr<QString> Log::makeRawString(int row) const {
  // qDebug() << "Log::makeRawString";
  auto line_pos = m_lines[row];
  int start = line_pos.first;
  int i = line_pos.second;

  int stringSize = int(i - start + 1);
  QByteArray localStr(stringSize, ' ');

  int insertPos = 0;
  for (int pos = start; pos < i; ++pos) {
    localStr[insertPos++] = m_bts[pos];
  }

  std::shared_ptr<QString> result =
      std::make_shared<QString>(m_codec->toUnicode(localStr));

  return result;
}

void Log::rawStringToValue(std::shared_ptr<QString> &rawString) const {
  if (m_global_highlight == nullptr) {
    throw std::logic_error("m_global_highlight==nullptr");
  }
  rawString->replace('<', "&lt;");
  rawString->replace('>', "&gt;");
  rawString->replace(' ', "&nbsp;"); // html eats white spaces

  for (auto it = m_global_highlight->begin(); it != m_global_highlight->end(); ++it) {
    heighlightStr(rawString.get(), *it);
  }
}

std::shared_ptr<QString> Log::makeString(int row) const {
  auto result = makeRawString(row);
  rawStringToValue(result);
  return result;
}
