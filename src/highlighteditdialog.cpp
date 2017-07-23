#include "highlighteditdialog.h"
#include "ui_highlighteditdialog.h"
#include <QDebug>


HighlightEditDialog::HighlightEditDialog(const HighlightPatterns &hl, QWidget *parent)
    : QDialog(parent), m_model(hl, parent), ui(new Ui::HighlightEditDialog) {
  ui->setupUi(this);
  ui->tableView->setModel(&m_model);

  connect(ui->actiondelete_selected, &QAction::triggered, this, &HighlightEditDialog::rmSelected);
  connect(ui->actionInsert_pattern, &QAction::triggered, this, &HighlightEditDialog::addNew);
}

HighlightEditDialog::~HighlightEditDialog() {
  delete ui;
}

void HighlightEditDialog::addNew() {
  qDebug() << "HighlightEditDialog::addNew()";
  m_model.insertRow(0);
}

void HighlightEditDialog::rmSelected() {
  qDebug() << "HighlightEditDialog::rmSelected()";
  auto selected=ui->tableView->selectionModel()->selectedIndexes();
  if(selected.size()!=0){
      for(auto v:selected){
          qDebug()<<v.row();
      }
      m_model.removeRows(selected.front().row(),1);
  }
}
