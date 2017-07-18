#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QListView>
#include <QStringListModel>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    calls++;
    auto model=new QStringListModel(this);
    QStringList list;
    for(int i=0;i<calls;++i){
        list << QString::number(i);
    }
    model->setStringList(list);

    auto lb=new QListView(m_tabbar);
    lb->setModel(model);
    m_tabbar->addTab(lb, "empty tab");
}

MainWindow::~MainWindow()
{
    delete ui;
}
