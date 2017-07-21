#include "highlighteditdialog.h"
#include "ui_highlighteditdialog.h"

HighlightEditDialog::HighlightEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HighlightEditDialog)
{
    ui->setupUi(this);
}

HighlightEditDialog::~HighlightEditDialog()
{
    delete ui;
}
