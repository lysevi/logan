#pragma once


#include <QMainWindow>
#include <QTabWidget>
#include <QTimer>
#include <QSettings>
#include "timerform.h"
#include "controller.h"
#include "logviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFile(const QString&fname);
    Log* getLog(int index);
    LogViewer*getViewer(int index);
    void endSearching();
public slots:
    //open,close,update
    void openFileSlot();
    void reloadCurentSlot();
    void reloadAllSlot();
    void closeCurentSlot();

    // settings
    void openFontDlgSlot();
    void selectTextEncodingSlot();
    void clearSettingsSlot();

    //view
    void autoscrollChangedSlot();
    void showToolbarSlot();
    void currentTabChangedSlot();

    //timer
    void timerIntervalChangedSlot(int v);
    void timerIntervalEnabledSlot(bool b);


    //search
    void showSearchPanelSlot();
    void searchPatternChangedSlot();
    void searchNextSlot();
    void searchEndSlot();
private:
    Ui::MainWindow *ui;
    QTabWidget *m_tabbar;
    Controller* m_controller;
    TimerForm *m_timer_widget;
    QTimer *m_timer;
    bool m_autoscroll_enabled;
    QSettings m_settings;
    QFont m_defaultFont;
    QString m_default_text_encoding;

    int m_search_index=0;
};
