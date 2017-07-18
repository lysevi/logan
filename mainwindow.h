#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QStringListModel>
#include "WindowController.h"
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
private:
    Ui::MainWindow *ui;
    QTabWidget *m_tabbar;
    WindowController* m_controller;
};

#endif // MAINWINDOW_H
