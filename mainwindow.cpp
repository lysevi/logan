#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logviewer.h"
#include "textcodecselectiondialog.h"
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QFontDialog>
#include <QMessageBox>
#include <QTextCodec>

const QString fontKey="logFont";
const QString showToolbarKey="showToolBar";
const QString defaultEncodingKey="defaultEncoding";

const QString v1=QString(LVIEW_VERSION);
const QString v2=QString(GIT_VERSION);
const QString logan_version="Logan - "+v1+"-"+v2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_timer(new QTimer(this)),
    m_settings("lysevi", "logan")
{
    m_timer->setInterval(0);

    ui->setupUi(this);

    m_defaultFont=this->font();
    if(m_settings.contains(fontKey))
    {
        QString fontName=m_settings.value(fontKey).toString();
        m_defaultFont=QFont(fontName);
        qDebug()<<"defaul font is "<<fontName;
    }

    if(m_settings.contains(showToolbarKey))
    {
        bool value=m_settings.value(showToolbarKey).toBool();
        ui->mainToolBar->setVisible(value);
        ui->actionshow_toolbar->setChecked(value);
    }

    m_default_text_encoding=defaultTextEncoding;
    if(m_settings.contains(defaultEncodingKey))
    {
        auto value=m_settings.value(defaultEncodingKey).toString();
        m_default_text_encoding=value;
        qDebug()<<"default encoding:"<<m_default_text_encoding;
    }

    setWindowIcon(QIcon(":/icons/logan.svg"));

    m_timer_widget=new TimerForm();

    ui->mainToolBar->addWidget(m_timer_widget);

    setMouseTracking(true);
    setAutoFillBackground(true);

    m_controller=new Controller(this);
    setWindowTitle(logan_version);
    m_tabbar=new QTabWidget(ui->centralWidget);
    m_tabbar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    ui->gridLayout->addWidget(m_tabbar);

    m_autoscroll_enabled=ui->actionautoscroll_enabled->isChecked();
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileSlot);
    connect(ui->actionreolad_current, &QAction::triggered, this, &MainWindow::reloadCurentSlot);
    connect(ui->actionclose_current_tab, &QAction::triggered, this, &MainWindow::closeCurentSlot);
    connect(ui->actionautoscroll_enabled, &QAction::triggered, this, &MainWindow::autoscrollChangedSlot);
    connect(ui->actionfont_selection, &QAction::triggered, this, &MainWindow::openFontDlgSlot);
    connect(ui->actionclearSettings, &QAction::triggered, this, &MainWindow::clearSettings);
    connect(ui->actionshow_toolbar, &QAction::triggered, this, &MainWindow::showToolbarSlot);
    connect(ui->actionselect_text_encoding, &QAction::triggered, this, &MainWindow::selectTextEncodingSlot);
    connect(m_timer_widget, &TimerForm::timerParamChangedSignal,this, &MainWindow::timerIntervalChangedSlot);
    connect(m_timer_widget, &TimerForm::timerIsEnabledSignal,this, &MainWindow::timerIntervalEnabledSlot);
    connect(m_timer, &QTimer::timeout,this, &MainWindow::reloadAllSlot);
    connect(m_tabbar, &QTabWidget::currentChanged, this, &MainWindow::currentTabChanged);
    m_timer_widget->defaultState();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete m_timer;
    delete m_controller;

}


void MainWindow::openFontDlgSlot(){
    qDebug()<<"openFontDlgSlot()";
    QFontDialog fdlg(this);
    if(!fdlg.exec()){
        return;
    }
    else{
        qDebug()<<"selected font: "<<fdlg.selectedFont().toString();
        m_settings.setValue(fontKey, fdlg.selectedFont().toString());
        setFont(QFont(fdlg.selectedFont().toString()));
    }
}

void MainWindow::openFile(const QString&fname){
    qDebug()<<"openFile()"<<fname;
    auto log=m_controller->openFile(m_default_text_encoding, fname);
    if(log==nullptr){
        return;
    }
    auto lb=new LogViewer(m_defaultFont, m_tabbar);
    lb->setModel(log);
    lb->setAutoScroll(m_autoscroll_enabled);
    m_tabbar->addTab(lb, log->filename());

    if(m_tabbar->count()==1)
    {
        m_tabbar->tabBar()->hide();
    }else{
        m_tabbar->tabBar()->setVisible(true);
    }
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
        openFile(v);
    }

}

void MainWindow::clearSettings(){
    qDebug()<<"clearSettings()";
    QMessageBox msgBox;

    msgBox.setInformativeText("Clear settings?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if(ret==QMessageBox::Yes)
    {
        qDebug()<<"clear settings...";
        m_settings.clear();
    }
}

void MainWindow::showToolbarSlot(){
    bool value=ui->actionshow_toolbar->isChecked();
    qDebug()<<"showToolbarSlot()"<<value;
    m_settings.setValue(showToolbarKey, value);
    ui->mainToolBar->setVisible(value);
}

void MainWindow::reloadCurentSlot(){
    //TODO refact!
    qDebug()<<"reloadCurentSlot()";
    int current=m_tabbar->currentIndex();
    if(current<0){
        return;
    }
    auto widget=m_tabbar->widget(current);
    auto log=dynamic_cast<LogViewer*>(widget)->model();
    m_controller->updateAllSlot(log->filename());
}

void MainWindow::closeCurentSlot(){
    //TODO refact!
    qDebug()<<"closeCurentSlot()";
    int current=m_tabbar->currentIndex();
    if(current<0){
        return;
    }
    auto widget=m_tabbar->widget(current);
    auto log=dynamic_cast<LogViewer*>(widget)->model();
    auto fname=log->filename();
    m_tabbar->removeTab(current);
    m_controller->closeFileSlot(fname);
}

void MainWindow::autoscrollChangedSlot(){
    qDebug()<<"autoscrollChangedSlot()"<<ui->actionautoscroll_enabled->isChecked();
    m_autoscroll_enabled=ui->actionautoscroll_enabled->isChecked();

    for(int i=0;i<m_tabbar->count();++i){
        auto widget=m_tabbar->widget(i);
        auto log=dynamic_cast<LogViewer*>(widget);
        log->setAutoScroll(m_autoscroll_enabled);
    }
}

void MainWindow::timerIntervalChangedSlot(int v){
    qDebug()<<"MainWindow::timerIntervalChangedSlot()"<<v;
    m_timer->setInterval(v*1000);

    qDebug()<<"interval: "<<m_timer->interval()<<"isActive:"<<m_timer->isActive();
}

void MainWindow::reloadAllSlot(){
    qDebug()<<"MainWindow::reloadAllSlot()";
    m_controller->updateAllSlot("null");
}

void MainWindow::timerIntervalEnabledSlot(bool b){
    if(b){
        m_timer->start();
    }else{
        m_timer->stop();
    }
}

void MainWindow::currentTabChanged(){
    qDebug()<<"MainWindow::currentTabChanged()";
    auto i=m_tabbar->currentIndex();
    if(i<0){
        return;
    }
    auto widget=m_tabbar->widget(i);
    auto log=dynamic_cast<LogViewer*>(widget);
    setWindowTitle(logan_version +": " + log->model()->filename());
}

void MainWindow::selectTextEncodingSlot(){
    qDebug()<<"MainWindow::selectTextEncodingSlot()";
    TextCodecSelectionDialog dlg(this);
    int ret=dlg.exec();
    qDebug()<<"ret:"<<ret;
    if(ret){
        auto encoding=dlg.selectedEncoding();
        qDebug()<<"selected encoding:"<<encoding;
        m_settings.setValue(defaultEncodingKey,encoding);
        m_default_text_encoding=encoding; //TODO set to all log files.
    }
}
