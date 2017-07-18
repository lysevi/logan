#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>
#include <QAbstractItemView>
#include "listboxeditableitem.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    setAutoFillBackground(true);
    m_controller=new WindowController(this);

    m_tabbar=new QTabWidget(ui->centralWidget);
    m_tabbar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    ui->gridLayout->addWidget(m_tabbar);
    //mainLayout = new QHBoxLayout;
   //auto layout=(QGridLayout*)ui->centralWidget->layout();
    //layout->addWidget(m_tabbar, Qt::AlignCenter);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileSlot);
}

void MainWindow::openFileSlot(){
    qDebug()<<"openFileSlot()";

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (!dialog.exec()){
        return;
    }
    fileNames = dialog.selectedFiles();
    for(auto v: fileNames){
        auto log=m_controller->openFile(v);
        if(log==nullptr){
            continue;
        }
        auto lb=new QListView(m_tabbar);
        lb->setAlternatingRowColors(true);
        lb->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked );
        lb->setModel(log);
        lb->setItemDelegate(new ListboxEditableItem(lb));
        m_tabbar->addTab(lb, v);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_controller;
}
