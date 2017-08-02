#include "mainwindow.h"
#include "highlighteditdialog.h"
#include "logviewer.h"
#include "pattern.h"
#include "textcodecselectiondialog.h"
#include "timeeditform.h"
#include "ui_mainwindow.h"
#include <QAbstractItemView>
#include <QDebug>
#include <QFileDialog>
#include <QFontDialog>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextCodec>

const QString version = QString(GIT_VERSION);
const QString logan_version = "Logan - " + version;
MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), _settings(new Settings(this)),
      m_timer(new QTimer(this)) {
  ui->setupUi(this);

  _recent_menu_addeded = false;
  _current_tab = 0;
  m_timer->setInterval(0);

  ui->searchFrame->setVisible(false);

  // ui settings
  setWindowIcon(QIcon(":/icons/logan.svg"));

  m_timer_widget = new TimerForm();

  ui->mainToolBar->addWidget(m_timer_widget);

  setMouseTracking(true);
  setAutoFillBackground(true);

  m_controller = new Controller(this);

  setWindowTitle(logan_version);
  m_tabbar = new QTabWidget(ui->centralWidget);
  m_tabbar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  ui->gridLayout->addWidget(m_tabbar);

  m_autoscroll_enabled = ui->actionautoscroll_enabled->isChecked();

  _filter_form = new FilterParamsForm(this);
  _filter_form->setVisible(false);
  ui->splitter->insertWidget(0, _filter_form);

  connect_signals();
  m_timer_widget->defaultState();

  _settings->load();
  QTimer::singleShot(300, this, SLOT(showMaximized()));
  instance = this;
}

MainWindow::~MainWindow() {
  _settings->save();

  delete ui;
  delete m_timer;
  delete m_controller;
}

Log *MainWindow::getLog(int index) {
  if (index < 0) {
    return nullptr;
  }
  auto widget = m_tabbar->widget(index);
  if (widget == nullptr) {
    return nullptr;
  }
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

    m_defaultFont = fdlg.selectedFont();
    _settings->saveFont();
    showMessageAboutSettings();
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
  connect(lb, &LogViewer::selectNewRow, this, &MainWindow::updateStatusBarInfoSlot);
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
  _settings->saveRecent();
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
    _settings->m_settings.clear();
  }
}

void MainWindow::showToolbarSlot() {
  bool value = ui->actionshow_toolbar->isChecked();
  qDebug() << "showToolbarSlot()" << value;
  _settings->m_settings.setValue(settings_keys::showToolbarKey, value);
  ui->mainToolBar->setVisible(value);
}

void MainWindow::reloadCurentSlot() {
  qDebug() << "reloadCurentSlot()";
  int current = m_tabbar->currentIndex();
  if (current < 0) {
    return;
  }
  auto log = getLog(current);
  m_controller->update(log->filename());
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
  m_controller->update();
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
}

void MainWindow::updateStatusBarInfoSlot() {
  qDebug() << "MainWindow::updateStatusBarInfoSlot()";
  auto i = m_tabbar->currentIndex();
  auto log = getLog(i);
  if (log != nullptr) {
    auto viewer = getViewer(i);
    auto msg1 = QString("%1/%0 %2")
                    .arg(QString::number(log->rowCount()))
                    .arg(viewer->selectedRow())
                    .arg(log->lastUpdate().toString());

    statusBar()->showMessage(msg1);
  } else {
    statusBar()->clearMessage();
  }
}

void MainWindow::selectTextEncodingSlot() {
  qDebug() << "MainWindow::selectTextEncodingSlot()";
  TextCodecSelectionDialog dlg(this);
  int ret = dlg.exec();
  qDebug() << "ret:" << ret;
  if (ret) {
    auto encoding = dlg.selectedEncoding();
    qDebug() << "selected encoding:" << encoding;
    _settings->m_settings.setValue(settings_keys::defaultEncodingKey, encoding);
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
  if (_filter_form->isVisible()) {
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

    _settings->saveHighlight();
    showMessageAboutSettings();
    m_controller->update("null");
  }
}

void MainWindow::exportHighlight() {
  qDebug() << "MainWindow::exportHighlight()";
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save highlight patterns"), "",
                                                  tr("Json (*.json);;All Files (*)"));
  if (!fileName.isEmpty()) {

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file"), file.errorString());
      return;
    } else {
      auto jsonStr = Settings::highlight2string(m_controller->m_global_highlight);
      file.write(jsonStr.toUtf8());
      file.close();
    }
  }
}

void MainWindow::importHighlight() {
  qDebug() << "MainWindow::importHighlight()";
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open highlight patterns"), "",
                                                  tr("Json (*.json);;All Files (*)"));

  if (!fileName.isEmpty()) {

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"), file.errorString());
      return;
    } else {
      QString jsonStr(file.readAll());
      auto subresult = Settings::highlightFromString(jsonStr);
      for (auto v : subresult) {
        m_controller->m_global_highlight.insert(v.pattern, v);
      }
      file.close();

      _settings->saveHighlight();
      auto message = tr("Imported %1 patterns.").arg(subresult.size());
      QMessageBox msgBox;

      msgBox.setInformativeText(message);
      msgBox.setIcon(QMessageBox::Icon::Information);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
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

  if (_recent_files.empty()) {
    return;
  }
  _recentFile_Menu.clear();
  if (!_recent_menu_addeded) {
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
    _recent_menu_addeded = true;
  }

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

void MainWindow::disableFiltration() {
  auto log = getLog(m_tabbar->currentIndex());
  if (log != nullptr) {
    log->clearFilter();
  }
}

void MainWindow::showFltrPanelSlot() {
  qDebug() << "MainWindow::showFltrPanelSlot()";
  bool isFltrVisible = _filter_form->isVisible();
  if (!isFltrVisible) {
    _filter_form->setVisible(true);
  } else {
    _filter_form->setVisible(false);
    disableFiltration();
  }
  updateStatusBarInfoSlot();
}

void MainWindow::filterApplySlot() {
  qDebug() << "MainWindow::filterApplySlot()";
  resetFilter();
}

void MainWindow::resetFilter() {
  auto log = getLog(m_tabbar->currentIndex());
  if (log != nullptr) {

    auto fltr = _filter_form->Filter();
    int fltrs_count = fltr->filters();

    if (fltrs_count == 0 || !_filter_form) {
      log->clearFilter();
      return;
    }

    if (fltrs_count != 0) {
      log->resetFilter(fltr);
    } else {
      log->clearFilter();
    }
    updateStatusBarInfoSlot();
  }
}

void MainWindow::showMessageAboutSettings() {
  auto message = tr("Changes will take effect for newly opened files");
  QMessageBox msgBox;

  msgBox.setInformativeText(message);
  msgBox.setIcon(QMessageBox::Icon::Information);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.exec();
}

void MainWindow::aboutProgram() {
  QMessageBox::about(this, tr("Logan"),
                     "<a href='https://github.com/lysevi/logan'>Log analyzer</a>");
}

void MainWindow::aboutQt() {
  QMessageBox::aboutQt(this);
}
