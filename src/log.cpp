#include "log.h"
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QPair>
#include <QRegExp>
//#include <QtConcurrent>
#include <QScrollBar>
#include <QTextCodec>

LinePositionList allLinePos(const QByteArray &bts) {

  int count = 0;
  auto size = bts.size();
  for (int i = 0; i < size; ++i) {
    if (bts[i] == '\n') {
      count++;
    }
  }
  LinePositionList result(count);
  int start = 0;
  int index = 0;
  for (int i = 0; i < size; ++i) {
    if (bts[i] == '\n') {
      LinePosition lp;
      lp.first = start;
      lp.second = i;
      lp.index = index++;
      result[lp.index] = lp;
      start = i + 1;
    }
  }
  return result;
}

Log *Log::openFile(const QString &fname, const HighlightPatterns *global_highlight,
                   const QString &default_encoding, QObject *parent) {
  qDebug() << "openFile" << fname;
  QFileInfo fileInfo(fname);
  auto ll = new Log(fileInfo, fname, global_highlight, default_encoding, parent);
  return ll;
}

void Log::loadFile() {
  qDebug() << "loadFile " << m_fname;
  auto curDT = QDateTime::currentDateTimeUtc();
  QFile inputFile(m_fname);
  if (inputFile.open(QIODevice::ReadOnly)) {
    m_bts = std::move(inputFile.readAll());
    m_lines = allLinePos(m_bts);
    m_cache.resize(m_lines.size());
    inputFile.close();
  } else {
    throw std::logic_error("file not exists!");
  }
  qDebug() << "elapsed time:" << curDT.secsTo(QDateTime::currentDateTimeUtc());
  m_load_complete = true;
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
  loadFile();
  auto idx = createIndex(-1, -1, nullptr);
  while (canFetchMore(idx)) {
    fetchMore(idx);
  }
  qDebug() << "loaded " << m_name << " lines:" << m_lines.size();
}

QString Log::name() const {
  return m_name;
}

QString Log::filename() const {
  return m_fname;
}

void Log::update() {
  qDebug() << "update " << m_name << "=>" << m_fname;
  QFileInfo fileInfo(m_fname);

  //    if(fileInfo.lastModified()<=m_lastModifed){
  //        qDebug()<<"nothing to read: curDt:"<<fileInfo.lastModified()<<"
  //        myDt:"<<m_fileInfo.lastModified();
  //        return;
  //    }
  m_lastModifed = fileInfo.lastModified();

  qDebug() << "loadFile " << m_fname;
  auto curDT = QDateTime::currentDateTimeUtc();
  QFile inputFile(m_fname);
  if (inputFile.open(QIODevice::ReadOnly)) {
    auto bts = inputFile.readAll();
    auto lines = allLinePos(bts);
    auto diff = lines.size() - m_lines.size();
    if (diff > 0) {
      auto old_size = m_lines.size();
      // m_load_complete=false;
      // this->beginResetModel();
      m_bts = std::move(bts);

      m_lines = std::move(lines);
      m_cache.clear();
      m_cache.resize(m_lines.size());
      m_load_complete = true;
      // this->endResetModel();

      emit countChanged(m_lines.size());
      emit linesChanged();

      //      int loaded = 0;
      //      std::map<int, CachedString> local_res;
      //      for (size_t i = 0; i < diff; ++i) {
      //        CachedString cs;
      //        cs.originValue = makeString(m_cache.size() + i);
      //        cs.Value = cs.originValue;
      //        local_res.insert(std::make_pair(m_cache.size() + i, cs));
      //        loaded++;
      //      }
      beginInsertRows(createIndex(0, 0, nullptr), old_size - 1, old_size + diff);
      //      for (auto &kv : local_res) {
      //        m_cache.insert(std::make_pair(kv.first, kv.second));
      //      }
      endInsertRows();
      emit m_lv_object->scrollToBottom();
    }

    inputFile.close();
  } else {
    throw std::logic_error("file not exists!");
  }

  qDebug() << "update elapsed time:" << curDT.secsTo(QDateTime::currentDateTimeUtc());
}

std::shared_ptr<QString> Log::makeRawString(int row) const {
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

int Log::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  if (!m_load_complete) {
    return 0;
  }
  if (_fltr != nullptr) {
    return m_cache.size();
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
    if (m_cache[index.row()].Value != nullptr) {
      return *m_cache[index.row()].Value;
    } else {
      CachedString cs;
      cs.originValue = makeString(index.row());
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
  return *makeRawString(index.row());
}

void Log::clearHightlight() {
  //    m_load_complete=false;
  //    this->beginResetModel();
  //    QtConcurrent::blockingMap(m_buffer,[](CachedString&cs){
  //        cs.Value=cs.originValue;
  //    });
  //    for(int k=0;k<m_buffer.size();++k){
  //        auto mi=this->createIndex(k,0);
  //        dataChanged(mi, mi);
  //    }
  //    m_load_complete=true;
  //    this->endResetModel();
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

void Log::updateHeighlights(QVector<CachedString>::iterator /*begin*/,
                            QVector<CachedString>::iterator /*end*/,
                            const QString &pattern) {
  if (pattern.size() == 0) {
    return;
  }
  auto curDT = QDateTime::currentDateTimeUtc();

  //    QtConcurrent::blockingMap(begin, end,[this,&pattern](CachedString&cs){
  //        QString str(*cs.Value);
  //        auto is_updated=heighlightStr(&str, pattern);
  //        if(is_updated){
  //            cs.Value=std::make_shared<QString>(str);
  //        }
  //    });
  //    for(int k=0;k<m_buffer.size();++k){
  //        auto mi=this->createIndex(k,0);
  //        dataChanged(mi, mi);
  //    }
  qDebug() << m_fname << "updateHeighlights elapsed time:"
           << curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Log::updateHeighlights(const QString & /*pattern*/) {
  // updateHeighlights(m_buffer.begin(), m_buffer.end(), pattern);
}

void Log::localHightlightPattern(const QString &pattern) {
  if (pattern.size() == 0) {
    return;
  }
  updateHeighlights(pattern);
}

void Log::setListVoxObject(QListView *object) {
  m_lv_object = object;
}

QPair<int, QString> Log::findFrom(const QString &pattern, int index,
                                  SearchDirection direction) {
  // TODO make thread safety. m_cache can be brokent, when timer is active
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

void Log::setFilter(const Filter_Ptr &fltr) {
  qDebug() << "Log::setFilter";
  _fltr = fltr;
  int count = 0;
  for (size_t i = 0; i < m_lines.size(); ++i) {
    auto qs = makeRawString(i);
    if (fltr->inFilter(*qs)) {
      rawStringToValue(qs);
      auto cs = m_cache[count];
      cs.clear();
      cs.index = count;
      cs.originValue = qs;
      cs.Value = cs.originValue;
      m_cache[count] = cs;
      count++;
    }
  }

  beginResetModel();
  m_cache.resize(count);
  endResetModel();
}

void Log::clearFilter() {
  qDebug() << "Log::clearFilter";
  _fltr = nullptr;
  beginResetModel();
  m_cache.resize(m_lines.size());
  for (int i = 0; i < m_cache.size(); ++i) {
    m_cache[i].clear();
  }
  _fltr = nullptr;
  endResetModel();
}
