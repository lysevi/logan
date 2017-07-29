#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::connect_signals() {
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
  connect(ui->actionExport_highlight_patterns, &QAction::triggered, this,
          &MainWindow::exportHighlight);
  connect(ui->actionImport_highlight_patterns, &QAction::triggered, this,
          &MainWindow::importHighlight);

  // timer
  connect(m_timer_widget, &TimerForm::timerParamChangedSignal, this,
          &MainWindow::timerIntervalChangedSlot);
  connect(m_timer_widget, &TimerForm::timerIsEnabledSignal, this,
          &MainWindow::timerIntervalEnabledSlot);

  connect(m_timer, &QTimer::timeout, this, &MainWindow::reloadAllSlot);
  connect(m_tabbar, &QTabWidget::currentChanged, this,
          &MainWindow::currentTabChangedSlot);
  connect(m_tabbar, &QTabWidget::currentChanged, this,
          &MainWindow::updateStatusBarInfoSlot);

  // search
  connect(ui->searchPatternEdit, &QLineEdit::textChanged, this,
          &MainWindow::searchPatternChangedSlot);
  connect(ui->searchNextPushButton, &QPushButton::clicked, this,
          &MainWindow::searchNextSlot);

  // fltr
  connect(ui->actionEnable_filtration, &QAction::triggered, this,
          &MainWindow::showFltrPanelSlot);
  connect(_filter_form, &FilterParamsForm::filterApply, this,
          &MainWindow::filterApplySlot);
}
