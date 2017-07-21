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
#include <QLineEdit>

#include "highlighteditdialog.h"

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

    ui->searchFrame->setVisible(false);

    //read settings
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

    //ui settings
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

    //actions
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileSlot);
    connect(ui->actionreolad_current, &QAction::triggered, this, &MainWindow::reloadCurentSlot);
    connect(ui->actionclose_current_tab, &QAction::triggered, this, &MainWindow::closeCurentSlot);
    connect(ui->actionautoscroll_enabled, &QAction::triggered, this, &MainWindow::autoscrollChangedSlot);
    connect(ui->actionfont_selection, &QAction::triggered, this, &MainWindow::openFontDlgSlot);
    connect(ui->actionclearSettings, &QAction::triggered, this, &MainWindow::clearSettingsSlot);
    connect(ui->actionshow_toolbar, &QAction::triggered, this, &MainWindow::showToolbarSlot);
    connect(ui->actionselect_text_encoding, &QAction::triggered, this, &MainWindow::selectTextEncodingSlot);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showSearchPanelSlot);
    connect(ui->actionsearch_end, &QAction::triggered, this, &MainWindow::searchEndSlot);
    connect(ui->actionHighlights, &QAction::triggered, this, &MainWindow::openHighlightDlg);

    connect(m_timer_widget, &TimerForm::timerParamChangedSignal,this, &MainWindow::timerIntervalChangedSlot);
    connect(m_timer_widget, &TimerForm::timerIsEnabledSignal,this, &MainWindow::timerIntervalEnabledSlot);

    connect(m_timer, &QTimer::timeout,this, &MainWindow::reloadAllSlot);
    connect(m_tabbar, &QTabWidget::currentChanged, this, &MainWindow::currentTabChangedSlot);

    connect(ui->searchPatternEdit, &QLineEdit::textChanged, this, &MainWindow::searchPatternChangedSlot);
    connect(ui->searchNextPushButton, &QPushButton::clicked, this, &MainWindow::searchNextSlot);


    m_timer_widget->defaultState();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete m_timer;
    delete m_controller;
}

Log* MainWindow::getLog(int index){
    auto widget=m_tabbar->widget(index);
    auto log=dynamic_cast<LogViewer*>(widget)->model();
    return log;
}

LogViewer*MainWindow::getViewer(int index){
    auto widget=m_tabbar->widget(index);
    auto lv=dynamic_cast<LogViewer*>(widget);
    return lv;
}

void MainWindow::openFontDlgSlot(){
    qDebug()<<"openFontDlgSlot() "<<m_defaultFont.toString();
    QFontDialog fdlg(m_defaultFont, this);

    if(!fdlg.exec()){
        return;
    }
    else{
        qDebug()<<"selected font: "<<fdlg.selectedFont().toString();
        m_settings.setValue(fontKey, fdlg.selectedFont().toString());
        m_defaultFont=fdlg.selectedFont();
        //setFont(QFont(fdlg.selectedFont().toString()));
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
    auto index=m_tabbar->addTab(lb, log->filename());
    m_tabbar->setCurrentIndex(index);

    if(m_tabbar->count()==1){
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

void MainWindow::clearSettingsSlot(){
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
    qDebug()<<"reloadCurentSlot()";
    int current=m_tabbar->currentIndex();
    if(current<0){
        return;
    }
    auto log=getLog(current);
    m_controller->updateAllSlot(log->filename());
}

void MainWindow::closeCurentSlot(){
    qDebug()<<"closeCurentSlot()";
    int current=m_tabbar->currentIndex();
    if(current<0){
        return;
    }
    auto log=getLog(current);
    auto fname=log->filename();
    m_tabbar->removeTab(current);
    m_controller->closeFileSlot(fname);

    if(ui->actionFind->isChecked()){
        ui->actionFind->trigger();
    }
}

void MainWindow::autoscrollChangedSlot(){
    qDebug()<<"autoscrollChangedSlot()"<<ui->actionautoscroll_enabled->isChecked();
    m_autoscroll_enabled=ui->actionautoscroll_enabled->isChecked();

    for(int i=0;i<m_tabbar->count();++i){
        auto log=getViewer(i);
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

void MainWindow::currentTabChangedSlot(){
    qDebug()<<"MainWindow::currentTabChanged()";
    auto i=m_tabbar->currentIndex();
    if(i<0){
        return;
    }
    auto log=getLog(i);
    setWindowTitle(logan_version +": " + log->filename());
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

void MainWindow::showSearchPanelSlot(){
    qDebug()<<"MainWindow::showSearchPanelSlot()";


    bool is_visible=ui->actionFind->isChecked();
    ui->searchFrame->setVisible(is_visible);
    if(is_visible)
    {
        ui->searchPatternEdit->setFocus();
        m_search_index=0;
        int current=m_tabbar->currentIndex();
        if(current>=0){
            auto log=getViewer(current);
            log->setAutoScroll(false);
        }
    }else
    {
        endSearching();
    }
}

void MainWindow::endSearching(){
    int current=m_tabbar->currentIndex();
    if(current>=0){
        auto log=getViewer(current);
        log->setAutoScroll(ui->actionautoscroll_enabled->isChecked());
    }
}

void MainWindow::searchPatternChangedSlot(){
    auto pattern=ui->searchPatternEdit->text();
    qDebug()<<"MainWindow::searchPatternChangedSlot() "<<pattern;
    m_search_index=0;
    searchNextSlot();
}

void MainWindow::searchNextSlot(){
    auto pattern=ui->searchPatternEdit->text();

    qDebug()<<"MainWindow::searchNextSlot() "<<pattern;

    int index=m_tabbar->currentIndex();
    if(index<0){
        return;
    }
    SearchDirection direction=ui->searchDownRadioButton->isChecked()?SearchDirection::Down:SearchDirection::Up;

    auto log=getLog(index);
    auto searchResult=log->findFrom(pattern,m_search_index, direction);
    qDebug()<<"result: "<<searchResult.first<<"=>"<<searchResult.second;
    if(searchResult.second.size()!=0){
        m_search_index=searchResult.first;
        auto log=getViewer(index);
        log->selectRow(m_search_index);
    }
}

void MainWindow::searchEndSlot(){
    qDebug()<<"MainWindow::searchEnd()";
    ui->actionFind->trigger();
}


void MainWindow::openHighlightDlg(){
    qDebug()<<"MainWindow::searchEndSlot()";
    HighlightEditDialog dlg(this);
    if(dlg.exec()){

    }
}
