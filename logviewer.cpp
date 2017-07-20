#include "logviewer.h"
#include "ui_logviewer.h"

LogViewer::LogViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogViewer)
{
    ui->setupUi(this);
    auto lb=ui->listView;
    lb->setAlternatingRowColors(true);
    lb->setItemDelegate(&m_delegate);
    lb->setSelectionMode(QAbstractItemView::SingleSelection);
    lb->installEventFilter(this);
    lb->setUniformItemSizes(true);
    lb->setLayoutMode(QListView::LayoutMode::Batched);
    lb->setSpacing(2);
}

LogViewer::~LogViewer(){
    delete ui;
}

void LogViewer::setModel(Log*model_){
  this->ui->listView->setModel(model_);
  model_->setListVoxObject(this->ui->listView);
  m_model=model_;
}
