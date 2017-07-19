#pragma once


#include <QMainWindow>
#include <QTabWidget>
#include <QTimer>
#include "timerform.h"
#include "controller.h"
#include "listboxeditableitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openFileSlot();
    void reloadCurentSlot();
    void reloadAllSlot();
    void closeCurentSlot();
    void timerIntervalChangedSlot(int v);
    void timerIntervalEnabledSlot(bool b);
private:
    Ui::MainWindow *ui;
    QTabWidget *m_tabbar;
    Controller* m_controller;
    ListboxEditableItem m_delegate;
    TimerForm *m_timer_widget;
    QTimer *m_timer;
};
