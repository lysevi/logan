#pragma once

#include "filter.h"
#include "timeeditform.h"
#include <QList>
#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class FilterParamsForm;
}

struct StringFilterDescription {
  bool is_enabled;
  QString pattern;
};

class FilterParamsForm : public QWidget {
  Q_OBJECT

public:
  explicit FilterParamsForm(QWidget *parent = 0);
  ~FilterParamsForm();

  void replaceTimeRangeWidgets();

  void update();
  Filter_Ptr Filter() const;

  QList<StringFilterDescription> m_filters;
signals:
  void filterApply();
public slots:
  void fltrItemChangedSlot(QStandardItem *item);
  void addFltrSlot();
  void rmSelectedFiltrSlot();
  void dateRangeChangedSlot();
  void applyFilterSlot();
private:
  Ui::FilterParamsForm *ui;

  TimeEditForm *fromTimeEdit, *toTimeEdit;

  std::shared_ptr<QStandardItemModel> m_filter_model;
};
