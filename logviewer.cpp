#include "logviewer.h"
#include "ui_logviewer.h"
#include <QScrollBar>
#include <QDebug>

LogViewer::LogViewer(const QFont&font,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogViewer),
    m_default_font(font)
{
    ui->setupUi(this);
    auto lb=ui->listView;

    m_delegate.m_default_font=m_default_font;

    lb->setAlternatingRowColors(true);
    lb->setItemDelegate(&m_delegate);
    lb->setSelectionMode(QAbstractItemView::SingleSelection);
    lb->installEventFilter(this);
    lb->setUniformItemSizes(true);
    lb->setLayoutMode(QListView::LayoutMode::Batched);
    lb->setSpacing(2);

    connect(lb->verticalScrollBar(), &QScrollBar::rangeChanged, this, LogViewer::onScrollRangeChanged);
}

LogViewer::~LogViewer(){
    delete ui;
}

void LogViewer::setModel(Log*model_){
  this->ui->listView->setModel(model_);
  model_->setListVoxObject(this->ui->listView);
  m_model=model_;
  connect(model_, SIGNAL(rowsInserted(QModelIndex,int,int)),
          this->ui->listView, SLOT(scrollToBottom()));
}

void LogViewer::setAutoScroll(bool value){
    m_autoscroll=value;
    if(value){
        ui->listView->scrollToBottom();
    }
}

void LogViewer::onScrollRangeChanged(int min, int max){
    Q_UNUSED(min);
    Q_UNUSED(max);
    if(m_autoscroll){
        ui->listView->scrollToBottom();
    }
}
