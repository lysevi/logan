#pragma once

#include "controller.h"
#include "logviewer.h"
#include "timerform.h"
#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QStringList>
#include <QStringListModel>
#include <QTabWidget>
#include <QTimer>

namespace Ui {
class MainWindow;
}

const int RecentFiles_Max = 10;
using RecentFiles = QVector<QString>;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void openFile(const QString &fname);
  Log *getLog(int index);
  LogViewer *getViewer(int index);
  void endSearching();
  void loadHighlightFromSettings();

  void updateRecentFileMenu();
  void saveRecent();
  void loadRecent();

  void disableFiltration();
  void resetFilter();
public slots:
  // open,close,update
  void openFileSlot();
  void reloadCurentSlot();
  void reloadAllSlot();
  void closeCurentSlot();
  void recentOpenSlot();

  // settings
  void openFontDlgSlot();
  void selectTextEncodingSlot();
  void clearSettingsSlot();

  // view
  void autoscrollChangedSlot();
  void showToolbarSlot();
  void currentTabChangedSlot();

  // timer
  void timerIntervalChangedSlot(int v);
  void timerIntervalEnabledSlot(bool b);

  // search
  void showSearchPanelSlot();
  void searchPatternChangedSlot();
  void searchNextSlot();
  void searchEndSlot();

  // highlights
  void openHighlightDlg();

  // filtration
  void showFltrPanelSlot();
  void addFltrSlot();
  void rmSelectedFiltrSlot();

private:
  Ui::MainWindow *ui;
  QTabWidget *m_tabbar;
  Controller *m_controller;
  TimerForm *m_timer_widget;
  QTimer *m_timer;
  bool m_autoscroll_enabled;
  QSettings m_settings, m_highlight_settings, m_recent_files_settings;
  QFont m_defaultFont;
  QString m_default_text_encoding;

  int m_search_index = 0;

  RecentFiles _recent_files;
  QMenu _recentFile_Menu;
  QVector<std::shared_ptr<QAction>> _recentFile_Actions;

  QStringList m_filters;
  QStringListModel m_filter_model;
};
