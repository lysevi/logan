#pragma once


#include <QMainWindow>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QStringListModel>
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
    void closeCurentSlot();
private:
    Ui::MainWindow *ui;
    QTabWidget *m_tabbar;
    Controller* m_controller;
    ListboxEditableItem m_delegate;
};
