#pragma once

#include "controller.h"
#include "filterparamsform.h"
#include "logviewer.h"
#include "timeeditform.h"
#include "timerform.h"
#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QStandardItemModel>
#include <QStringList>
#include <QTabWidget>
#include <QTimer>

namespace Ui {
class MainWindow;
}

namespace settings_keys {
const QString fontKey = "logFont";
const QString showToolbarKey = "showToolBar";
const QString defaultEncodingKey = "defaultEncoding";
const QString highlightKey = "highlightKey";
const QString recentFilesKey = "recentFilesKey";
const QString filtersKey = "filtersKey";
const QString filterFrameWidthKey = "filterFrameWidthKey";
}

const int RecentFiles_Max = 10;
using RecentFiles = QVector<QString>;

class MainWindow;
struct Settings {
  Settings(MainWindow *window);

  void load();
  void save();

  void saveFont();

  void saveHighlight();
  void loadHighlight();

  void saveFilters();
  void loadFilters();

  void saveLayout();
  void loadLayout();

  void saveRecent();
  void loadRecent();

  static QString highlight2string(const HighlightPatterns &hm);
  static HighlightPatterns highlightFromString(const QString &jsonStr);

  MainWindow *_mainWindow;
  QSettings m_settings, m_highlight_settings, m_recent_files_settings, m_filters_settings;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  friend class Settings;
  static MainWindow *instance;
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void openFile(const QString &fname);
  Log *getLog(int index);
  LogViewer *getViewer(int index);

  void connect_signals();

  void endSearching();

  void updateRecentFileMenu();

  void fillFilterModel();
  void disableFiltration();
  void resetFilter();

  void showMessageAboutSettings();
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
  void exportHighlight();
  void importHighlight();

  // filtration
  void showFltrPanelSlot();

  // statusbar
  void updateStatusBarInfoSlot();
  void filterApplySlot();

  // about
  void aboutProgram();
  void aboutQt();

private:
  int _current_tab;
  Ui::MainWindow *ui;
  std::unique_ptr<Settings> _settings;
  QTabWidget *m_tabbar;
  Controller *m_controller;
  TimerForm *m_timer_widget;
  QTimer *m_timer;
  bool m_autoscroll_enabled;

  QFont m_defaultFont;
  QString m_default_text_encoding;

  int m_search_index = 0;

  bool _recent_menu_addeded;
  RecentFiles _recent_files;
  QMenu _recentFile_Menu;
  QVector<std::shared_ptr<QAction>> _recentFile_Actions;

  FilterParamsForm *_filter_form;
};
