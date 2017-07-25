#include "mainwindow.h"
#include "highlighteditdialog.h"
#include "pattern.h"
#include "logviewer.h"
#include "textcodecselectiondialog.h"
#include "ui_mainwindow.h"
#include <QAbstractItemView>
#include <QDebug>
#include <QFileDialog>
#include <QFontDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextCodec>

namespace settings_keys {
const QString fontKey = "logFont";
const QString showToolbarKey = "showToolBar";
const QString defaultEncodingKey = "defaultEncoding";
const QString highlightKey = "highlightKey";
const QString recentFilesKey = "recentFilesKey";
const QString filtersKey = "filtersKey";
}

const QString version = QString(GIT_VERSION);
const QString logan_version = "Logan - " + version;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_timer(new QTimer(this)),
      m_settings("lysevi", "logan"), m_highlight_settings("lysevi", "logan_highlights"),
      m_recent_files_settings("lysevi", "logan_highlights"),
      m_filters_settings("lysevi", "logan_filters"), _recent_files() {
  _current_tab = 0;
  m_timer->setInterval(0);

  ui->setupUi(this);

  ui->searchFrame->setVisible(false);
  ui->fltrFrame->setVisible(false);

  // read settings
  m_defaultFont = this->font();
  if (m_settings.contains(settings_keys::fontKey)) {
    QString fontName = m_settings.value(settings_keys::fontKey).toString();
    m_defaultFont = QFont(fontName);
    qDebug() << "defaul font is " << fontName;
  }

  if (m_settings.contains(settings_keys::showToolbarKey)) {
    bool value = m_settings.value(settings_keys::showToolbarKey).toBool();
    ui->mainToolBar->setVisible(value);
    ui->actionshow_toolbar->setChecked(value);
  }

  m_default_text_encoding = defaultTextEncoding;
  if (m_settings.contains(settings_keys::defaultEncodingKey)) {
    auto value = m_settings.value(settings_keys::defaultEncodingKey).toString();
    m_default_text_encoding = value;
    qDebug() << "default encoding:" << m_default_text_encoding;
  }

  // ui settings
  setWindowIcon(QIcon(":/icons/logan.svg"));

  m_timer_widget = new TimerForm();

  ui->mainToolBar->addWidget(m_timer_widget);

  setMouseTracking(true);
  setAutoFillBackground(true);

  m_controller = new Controller(this);
  loadHighlightFromSettings();
  setWindowTitle(logan_version);
  m_tabbar = new QTabWidget(ui->centralWidget);
  m_tabbar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  ui->gridLayout->addWidget(m_tabbar);

  m_autoscroll_enabled = ui->actionautoscroll_enabled->isChecked();

  // actions
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileSlot);
  connect(ui->actionreolad_current, &QAction::triggered, this,
          &MainWindow::reloadCurentSlot);
  connect(ui->actionclose_current_tab, &QAction::triggered, this,
          &MainWindow::closeCurentSlot);
  connect(ui->actionautoscroll_enabled, &QAction::triggered, this,
          &MainWindow::autoscrollChangedSlot);
  connect(ui->actionfont_selection, &QAction::triggered, this,
          &MainWindow::openFontDlgSlot);
  connect(ui->actionclearSettings, &QAction::triggered, this,
          &MainWindow::clearSettingsSlot);
  connect(ui->actionshow_toolbar, &QAction::triggered, this,
          &MainWindow::showToolbarSlot);
  connect(ui->actionselect_text_encoding, &QAction::triggered, this,
          &MainWindow::selectTextEncodingSlot);
  connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showSearchPanelSlot);
  connect(ui->actionsearch_end, &QAction::triggered, this, &MainWindow::searchEndSlot);
  connect(ui->actionHighlights, &QAction::triggered, this, &MainWindow::openHighlightDlg);

  // timer
  connect(m_timer_widget, &TimerForm::timerParamChangedSignal, this,
          &MainWindow::timerIntervalChangedSlot);
  connect(m_timer_widget, &TimerForm::timerIsEnabledSignal, this,
          &MainWindow::timerIntervalEnabledSlot);

  connect(m_timer, &QTimer::timeout, this, &MainWindow::reloadAllSlot);
  connect(m_tabbar, &QTabWidget::currentChanged, this,
          &MainWindow::currentTabChangedSlot);

  // search
  connect(ui->searchPatternEdit, &QLineEdit::textChanged, this,
          &MainWindow::searchPatternChangedSlot);
  connect(ui->searchNextPushButton, &QPushButton::clicked, this,
          &MainWindow::searchNextSlot);

  // fltr
  connect(ui->actionEnable_filtration, &QAction::triggered, this,
          &MainWindow::showFltrPanelSlot);
  connect(ui->addFltrButton, &QPushButton::clicked, this, &MainWindow::addFltrSlot);
  connect(ui->rmFltrButton, &QPushButton::clicked, this,
          &MainWindow::rmSelectedFiltrSlot);

  connect(ui->timeRangeGroupBox, &QGroupBox::toggled, this,
          &MainWindow::dateRangeChangedSlot);
  connect(ui->rangeFrom, &QTimeEdit::timeChanged, this,
          &MainWindow::dateRangeChangedSlot);
  connect(ui->rangeTo, &QTimeEdit::timeChanged, this, &MainWindow::dateRangeChangedSlot);

  m_timer_widget->defaultState();

  loadRecent();
  loadFiltersFromSettings();
  QTimer::singleShot(300, this, SLOT(showMaximized()));
}

MainWindow::~MainWindow() {
  saveFiltersSettings();

  delete ui;
  delete m_timer;
  delete m_controller;
}

Log *MainWindow::getLog(int index) {
  if (index < 0) {
    return nullptr;
  }
  auto widget = m_tabbar->widget(index);
  auto log = dynamic_cast<LogViewer *>(widget)->model();
  return log;
}

LogViewer *MainWindow::getViewer(int index) {
  auto widget = m_tabbar->widget(index);
  auto lv = dynamic_cast<LogViewer *>(widget);
  return lv;
}

void MainWindow::openFontDlgSlot() {
  qDebug() << "openFontDlgSlot() " << m_defaultFont.toString();
  QFontDialog fdlg(m_defaultFont, this);

  if (!fdlg.exec()) {
    return;
  } else {
    qDebug() << "selected font: " << fdlg.selectedFont().toString();
    m_settings.setValue(settings_keys::fontKey, fdlg.selectedFont().toString());
    m_defaultFont = fdlg.selectedFont();
    // setFont(QFont(fdlg.selectedFont().toString()));
  }
}

void MainWindow::openFile(const QString &fname) {
  qDebug() << "openFile()" << fname;
  auto log = m_controller->openFile(m_default_text_encoding, fname);
  if (log == nullptr) {
    return;
  }

  auto lb = new LogViewer(m_defaultFont, m_tabbar);
  lb->setModel(log);
  lb->setAutoScroll(m_autoscroll_enabled);
  auto index = m_tabbar->addTab(lb, log->filename());
  m_tabbar->setCurrentIndex(index);

  if (m_tabbar->count() == 1) {
    m_tabbar->tabBar()->hide();
  } else {
    m_tabbar->tabBar()->setVisible(true);
  }

  bool isExists = false;
  for (int i = 0; i < _recent_files.size(); ++i) {
    if (_recent_files[i] == fname) {
      auto s = _recent_files[i];
      _recent_files.remove(i);
      _recent_files.insert(0, s);
      isExists = true;
      break;
    }
  }
  if (!isExists) {
    _recent_files.append(fname);
    if (_recent_files.size() > RecentFiles_Max) {
      _recent_files.remove(0);
    }
  }
  saveRecent();
  updateRecentFileMenu();
}

void MainWindow::openFileSlot() {
  qDebug() << "openFileSlot()";

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setViewMode(QFileDialog::Detail);
  QStringList fileNames;
  if (!dialog.exec()) {
    return;
  }
  fileNames = dialog.selectedFiles();
  for (auto v : fileNames) {
    openFile(v);
  }
}

void MainWindow::clearSettingsSlot() {
  qDebug() << "clearSettings()";
  QMessageBox msgBox;

  msgBox.setInformativeText("Clear settings?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Yes) {
    qDebug() << "clear settings...";
    m_settings.clear();
  }
}

void MainWindow::showToolbarSlot() {
  bool value = ui->actionshow_toolbar->isChecked();
  qDebug() << "showToolbarSlot()" << value;
  m_settings.setValue(settings_keys::showToolbarKey, value);
  ui->mainToolBar->setVisible(value);
}

void MainWindow::reloadCurentSlot() {
  qDebug() << "reloadCurentSlot()";
  int current = m_tabbar->currentIndex();
  if (current < 0) {
    return;
  }
  auto log = getLog(current);
  m_controller->updateAllSlot(log->filename());
}

void MainWindow::closeCurentSlot() {
  qDebug() << "closeCurentSlot()";
  int current = m_tabbar->currentIndex();
  if (current < 0) {
    return;
  }
  auto log = getLog(current);
  auto fname = log->filename();
  m_tabbar->removeTab(current);
  m_controller->closeFileSlot(fname);

  if (ui->actionFind->isChecked()) {
    ui->actionFind->trigger();
  }
}

void MainWindow::autoscrollChangedSlot() {
  qDebug() << "autoscrollChangedSlot()" << ui->actionautoscroll_enabled->isChecked();
  m_autoscroll_enabled = ui->actionautoscroll_enabled->isChecked();

  for (int i = 0; i < m_tabbar->count(); ++i) {
    auto log = getViewer(i);
    log->setAutoScroll(m_autoscroll_enabled);
  }
}

void MainWindow::timerIntervalChangedSlot(int v) {
  qDebug() << "MainWindow::timerIntervalChangedSlot()" << v;
  m_timer->setInterval(v * 1000);

  qDebug() << "interval: " << m_timer->interval() << "isActive:" << m_timer->isActive();
}

void MainWindow::reloadAllSlot() {
  qDebug() << "MainWindow::reloadAllSlot()";
  m_controller->updateAllSlot("null");
}

void MainWindow::timerIntervalEnabledSlot(bool b) {
  if (b) {
    m_timer->start();
  } else {
    m_timer->stop();
  }
}

void MainWindow::currentTabChangedSlot() {
  qDebug() << "MainWindow::currentTabChanged()";
  auto log = getLog(_current_tab);
  if (log != nullptr) {
    log->clearFilter();
  }
  auto i = m_tabbar->currentIndex();
  if (i < 0) {
    return;
  }
  log = getLog(i);
  setWindowTitle(logan_version + ": " + log->filename());
  _current_tab = i;
  resetFilter();
}

void MainWindow::selectTextEncodingSlot() {
  qDebug() << "MainWindow::selectTextEncodingSlot()";
  TextCodecSelectionDialog dlg(this);
  int ret = dlg.exec();
  qDebug() << "ret:" << ret;
  if (ret) {
    auto encoding = dlg.selectedEncoding();
    qDebug() << "selected encoding:" << encoding;
    m_settings.setValue(settings_keys::defaultEncodingKey, encoding);
    m_default_text_encoding = encoding;
  }
}

void MainWindow::showSearchPanelSlot() {
  qDebug() << "MainWindow::showSearchPanelSlot()";

  bool is_visible = ui->actionFind->isChecked();
  ui->searchFrame->setVisible(is_visible);
  if (is_visible) {
    ui->searchPatternEdit->setFocus();
    m_search_index = 0;
    int current = m_tabbar->currentIndex();
    if (current >= 0) {
      auto log = getViewer(current);
      log->setAutoScroll(false);
    }
  } else {
    endSearching();
  }
}

void MainWindow::endSearching() {
  int current = m_tabbar->currentIndex();
  if (current >= 0) {
    auto log = getViewer(current);
    log->setAutoScroll(ui->actionautoscroll_enabled->isChecked());
  }
}

void MainWindow::searchPatternChangedSlot() {
  auto pattern = ui->searchPatternEdit->text();
  qDebug() << "MainWindow::searchPatternChangedSlot() " << pattern;
  m_search_index = 0;
  searchNextSlot();
}

void MainWindow::searchNextSlot() {
  auto pattern = ui->searchPatternEdit->text();

  qDebug() << "MainWindow::searchNextSlot() " << pattern;

  int index = m_tabbar->currentIndex();
  if (index < 0) {
    return;
  }
  SearchDirection direction = ui->searchDownRadioButton->isChecked()
                                  ? SearchDirection::Down
                                  : SearchDirection::Up;

  auto log = getLog(index);
  auto searchResult = log->findFrom(pattern, m_search_index, direction);
  qDebug() << "result: " << searchResult.first << "=>" << searchResult.second;
  if (searchResult.second.size() != 0) {
    m_search_index = searchResult.first;
    auto log = getViewer(index);
    log->selectRow(m_search_index);
  }
}

void MainWindow::searchEndSlot() {
  qDebug() << "MainWindow::searchEnd()";
  if (ui->searchFrame->isVisible()) {
    ui->actionFind->trigger();
  }
  if (ui->fltrFrame->isVisible()) {
    ui->actionEnable_filtration->trigger();
  }
}

void MainWindow::openHighlightDlg() {
  qDebug() << "MainWindow::searchEndSlot()";
  HighlightEditDialog dlg(m_controller->m_global_highlight, this);
  if (dlg.exec()) {
    m_controller->m_global_highlight = dlg.m_model.result();

    for (auto v : m_controller->m_global_highlight) {
      qDebug() << v.pattern << "=>" << v.rgb;
    }

    saveHighlightFromSettings();

    m_controller->updateAllSlot("null");
  }
}

void MainWindow::saveHighlightFromSettings() {
  QJsonObject json;
  QJsonArray values;
  auto patterns = m_controller->m_global_highlight.values();
  for (auto v : patterns) {
    QJsonObject js_v;
    js_v["pattern"] = v.pattern;
    js_v["rgb"] = v.rgb;
    values.append(js_v);
  }
  json["patterns"] = values;
  QJsonDocument doc(json);
  QString strJson(doc.toJson(QJsonDocument::Compact));
  m_highlight_settings.setValue(settings_keys::highlightKey, strJson);
  qDebug() << ">>> " << strJson;
}

void MainWindow::loadHighlightFromSettings() {
  qDebug() << "MainWindow::loadHighlightFromSettings()";

  if (m_highlight_settings.contains(settings_keys::highlightKey)) {
    m_controller->m_global_highlight.clear();
    QString jsonStr = m_highlight_settings.value(settings_keys::highlightKey).toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isNull()) {
      if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QJsonArray patterns = obj["patterns"].toArray();
        for (int i = 0; i < patterns.count(); ++i) {
          auto p = patterns.at(i).toObject();
          HighlightPattern hp;
          hp.pattern = p["pattern"].toString();
          hp.rgb = p["rgb"].toString();
          qDebug() << ">> " << hp.pattern << ":" << hp.rgb;
          m_controller->m_global_highlight.insert(hp.pattern, hp);
        }
      }
    } else {
      throw std::logic_error("highlights format error: " + jsonStr.toStdString());
    }
  } else {
    m_controller->m_global_highlight = default_highlight_settings;
  }
}

void MainWindow::saveFiltersSettings() {
  qDebug() << "MainWindow::saveFiltersSettings()";
  QJsonObject json;
  QJsonArray values;
  auto patterns = m_controller->m_global_highlight.values();
  for (auto v : m_filters) {
    QJsonObject js_v;
    js_v["pattern"] = v.pattern;
    js_v["enabled"] = v.is_enabled;
    values.append(js_v);
  }
  json["patterns"] = values;
  QJsonDocument doc(json);
  QString strJson(doc.toJson(QJsonDocument::Compact));
  m_filters_settings.setValue(settings_keys::filtersKey, strJson);
  qDebug() << ">>> " << strJson;
}

void MainWindow::loadFiltersFromSettings() {
  qDebug() << "MainWindow::loadFiltersFromSettings()";
  if (m_filters_settings.contains(settings_keys::filtersKey)) {
    QString jsonStr = m_filters_settings.value(settings_keys::filtersKey).toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isNull()) {
      if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QJsonArray patterns = obj["patterns"].toArray();
        for (int i = 0; i < patterns.count(); ++i) {
          auto p = patterns.at(i).toObject();
          StringFilterDescription sfd;
          sfd.pattern = p["pattern"].toString();
          sfd.is_enabled = p["enabled"].toBool();
          qDebug() << ">> " << sfd.pattern << ":" << sfd.is_enabled;
          m_filters << sfd;
        }
      }
    } else {
      throw std::logic_error("filters format error: " + jsonStr.toStdString());
    }
  }
}

void MainWindow::recentOpenSlot() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    auto fname = action->data().toString();
    qDebug() << "open:" << fname;
    openFile(fname);
  }
}

void MainWindow::updateRecentFileMenu() {
  qDebug() << "MainWindow::updateRecentFileMenu()";

  _recentFile_Menu.clear();
  _recentFile_Menu.setTitle(tr("Recent files"));

  QAction *delim = nullptr;
  int index = 0;
  for (auto act : ui->menuFile->actions()) {
    qDebug() << act->text();
    if (act->text() == "") {
      index++;
      if (index == 2) { // find second delim (before "exit")
        delim = act;
        break;
      }
    }
  }
  ui->menuFile->insertMenu(delim, &_recentFile_Menu);
  _recentFile_Actions.clear();
  _recentFile_Actions.resize(_recent_files.size());
  int i = 0;
  for (const auto &fname : _recent_files) {
    if (fname != "") {
      QString text = tr("&%1: %2").arg(i + 1).arg(fname);
      _recentFile_Actions[i] = std::make_shared<QAction>(this);
      _recentFile_Actions[i]->setText(text);
      _recentFile_Actions[i]->setData(fname);
      _recentFile_Actions[i]->setVisible(true);
      _recentFile_Menu.addAction(_recentFile_Actions[i].get());
      connect(_recentFile_Actions[i].get(), &QAction::triggered, this,
              &MainWindow::recentOpenSlot);
      i++;
    }
  }
}

void MainWindow::saveRecent() {
  qDebug() << "MainWindow::saveRecent()";
  if (_recent_files.size() > 0) {
    m_recent_files_settings.beginWriteArray(settings_keys::recentFilesKey);
    for (int i = 0; i < _recent_files.size(); ++i) {
      m_recent_files_settings.setArrayIndex(i);
      m_recent_files_settings.setValue("file", _recent_files[i]);
    }
    m_recent_files_settings.endArray();
  }
}

void MainWindow::loadRecent() {
  qDebug() << "MainWindow::loadRecent()";
  int sz = m_recent_files_settings.beginReadArray(settings_keys::recentFilesKey);
  for (int i = 0; i < sz; ++i) {
    m_recent_files_settings.setArrayIndex(i);
    auto fname = m_recent_files_settings.value("file").toString();
    QFileInfo check_file(fname);
    if (check_file.exists() && check_file.isFile()) {
      _recent_files.append(fname);
    } else {
      qDebug() << fname << "not exists or not a file";
    }
  }
  m_recent_files_settings.endArray();
  updateRecentFileMenu();
  saveRecent();
}

void MainWindow::disableFiltration() {
  auto log = getLog(m_tabbar->currentIndex());
  if (log != nullptr) {
    log->clearFilter();
  }
}

void MainWindow::showFltrPanelSlot() {
  qDebug() << "MainWindow::showFltrPanelSlot()";
  bool isFltrVisible = ui->fltrFrame->isVisible();
  if (!isFltrVisible) {
    ui->fltrFrame->setVisible(true);
    ui->fltrEdit->setFocus();
    ui->timeRangeGroupBox->setChecked(false);
    fillFilterModel();
    resetFilter();
  } else {
    ui->fltrFrame->setVisible(false);
    disableFiltration();
  }
}

void MainWindow::addFltrSlot() {
  qDebug() << "MainWindow::addFltrSlot()";
  auto text = ui->fltrEdit->text();
  if (text.trimmed() != "") {
    m_filters << StringFilterDescription{true, text.trimmed()};
    fillFilterModel();

    resetFilter();
  }
  ui->fltrEdit->clear();
}

void MainWindow::rmSelectedFiltrSlot() {
  qDebug() << "MainWindow::rmSelectedFiltrSlot()";
  auto selectedIndexes = ui->filtrListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.size() > 0) {
    m_filters.removeAt(selectedIndexes.front().row());
    fillFilterModel();
    resetFilter();
  }
}

void MainWindow::dateRangeChangedSlot() {
  qDebug() << "MainWindow::dateRangeChangedSlot()";
  resetFilter();
}

void MainWindow::fillFilterModel() {
  if (m_filter_model != nullptr) {
    for (int i = 0; i < m_filter_model->rowCount(); ++i) {
      delete m_filter_model->item(i);
    }
    m_filter_model->clear();
  }
  m_filter_model = std::make_shared<QStandardItemModel>(ui->filtrListView);
  ui->filtrListView->setModel(m_filter_model.get());
  int row = 0;
  for (auto fltr : m_filters) {
    QStandardItem *item = new QStandardItem(fltr.pattern);
    item->setCheckable(true);
    item->setCheckState(fltr.is_enabled ? Qt::Checked : Qt::Unchecked);
    item->setData(Qt::Checked, Qt::CheckStateRole);
    m_filter_model->setItem(row++, item);
  }
  connect(m_filter_model.get(), &QStandardItemModel::itemChanged, this,
          &MainWindow::fltrItemChangedSlot);
}

void MainWindow::fltrItemChangedSlot(QStandardItem *item) {
  for (auto &v : m_filters) {
    if (v.pattern.trimmed() == item->text().trimmed()) {
      v.is_enabled = item->checkState() == Qt::Checked;
      qDebug() << v.pattern << " => " << v.is_enabled;
    }
  }
  resetFilter();
}

void MainWindow::resetFilter() {
  auto log = getLog(m_tabbar->currentIndex());
  if (log != nullptr) {

    auto fltr = std::make_shared<FilterUnion>();
    size_t fltrs_count = 0;

    if (ui->timeRangeGroupBox->isChecked()) {
      auto timeFltr =
          std::make_shared<DateRangeFilter>(ui->rangeFrom->time(), ui->rangeTo->time());
      fltr->addFilter(timeFltr);
      fltrs_count++;
    }

    for (const auto &s : m_filters) {
      qDebug() << s.pattern << " => " << s.is_enabled;
      if (s.is_enabled) {
        fltrs_count++;
        fltr->addFilter(std::make_shared<StringFilter>(s.pattern));
      }
    }

    if (fltrs_count == 0 || !ui->fltrFrame->isVisible()) {
      log->clearFilter();
      return;
    }

    if (fltrs_count != 0) {
      log->resetFilter(fltr);
    } else {
      log->clearFilter();
    }
  }
}
