#ifndef HIGHLIGHTEDITDIALOG_H
#define HIGHLIGHTEDITDIALOG_H

#include <QDialog>
#include <QList>
#include "log.h"

namespace Ui {
class HighlightEditDialog;
}



class HighlightEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HighlightEditDialog(QWidget *parent = 0);
    ~HighlightEditDialog();

private:
    Ui::HighlightEditDialog *ui;
};

#endif // HIGHLIGHTEDITDIALOG_H
