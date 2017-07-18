#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>
#include <QAbstractItemView>

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

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileSlot);
    connect(ui->actionreolad_current, &QAction::triggered, this, &MainWindow::reloadCurentSlot);
    connect(ui->actionclose_current_tab, &QAction::triggered, this, &MainWindow::closeCurentSlot);
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
        lb->setModel(log);
        log->setListVoxObject(lb);
        lb->setItemDelegate(&m_delegate);
        lb->setSelectionMode(QAbstractItemView::SingleSelection);
        lb->installEventFilter(this);
        m_tabbar->addTab(lb, v);
    }
}

void MainWindow::reloadCurentSlot(){
    //TODO refact!
    qDebug()<<"reloadCurentSlot()";
    int current=m_tabbar->currentIndex();
    auto widget=m_tabbar->widget(current);
    auto model=static_cast<QListView*>(widget)->model();
    auto log=static_cast<Log*>(model);
    m_controller->updateAllSlot(log->filename());
}

void MainWindow::closeCurentSlot(){
    //TODO refact!
    qDebug()<<"closeCurentSlot()";
    int current=m_tabbar->currentIndex();
    auto widget=m_tabbar->widget(current);
    auto model=static_cast<QListView*>(widget)->model();
    auto log=static_cast<Log*>(model);
    auto fname=log->filename();
    m_tabbar->removeTab(current);
    m_controller->closeFileSlot(fname);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_controller;
}
