#ifndef HIGHLIGHTEDITDIALOG_H
#define HIGHLIGHTEDITDIALOG_H

#include "highlightmodel.h"
#include "log.h"
#include <QDialog>
#include <QList>

namespace Ui {
class HighlightEditDialog;
}

class HighlightEditDialog : public QDialog {
  Q_OBJECT

public:
  explicit HighlightEditDialog(const HighlightPatterns &hl, QWidget *parent = 0);
  ~HighlightEditDialog();

  HighlightModel m_model;
public slots:
  void addNew();
  void rmSelected();
  void setToDefault();

private:
  Ui::HighlightEditDialog *ui;
};

#endif // HIGHLIGHTEDITDIALOG_H
