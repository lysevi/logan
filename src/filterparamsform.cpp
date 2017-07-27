#include "filterparamsform.h"
#include "ui_filterparamsform.h"
#include <QDebug>

FilterParamsForm::FilterParamsForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::FilterParamsForm) {
  ui->setupUi(this);
  ui->timeRangeGroupBox->setChecked(false);
  replaceTimeRangeWidgets();

  connect(ui->addFltrButton, &QPushButton::clicked, this, &FilterParamsForm::addFltrSlot);
  connect(ui->rmFltrButton, &QPushButton::clicked, this,
          &FilterParamsForm::rmSelectedFiltrSlot);

  connect(ui->timeRangeGroupBox, &QGroupBox::toggled, this,
          &FilterParamsForm::dateRangeChangedSlot);
  connect(fromTimeEdit, &TimeEditForm::timeChanged, this,
          &FilterParamsForm::dateRangeChangedSlot);
  connect(toTimeEdit, &TimeEditForm::timeChanged, this,
          &FilterParamsForm::dateRangeChangedSlot);
  connect(ui->applyFilterBtn, &QPushButton::clicked, this,
          &FilterParamsForm::applyFilterSlot);
}

FilterParamsForm::~FilterParamsForm() {
  delete ui;
}

void FilterParamsForm::replaceTimeRangeWidgets() {
  {
    auto ly = ui->timeRangeGroupBox->layout();
    auto oldFrom = ui->rangeFrom;
    fromTimeEdit = new TimeEditForm(TIME_RANGE::MIN, ui->timeRangeGroupBox);
    ly->replaceWidget(oldFrom, fromTimeEdit);
    ui->rangeFrom = fromTimeEdit;
    delete oldFrom;
  }
  {
    auto ly = ui->timeRangeGroupBox->layout();
    auto oldTo = ui->rangeTo;
    toTimeEdit = new TimeEditForm(TIME_RANGE::MAX, ui->timeRangeGroupBox);
    ly->replaceWidget(oldTo, toTimeEdit);
    ui->rangeTo = toTimeEdit;
    delete oldTo;
  }
}

void FilterParamsForm::fltrItemChangedSlot(QStandardItem *item) {
  for (auto &v : m_filters) {
    if (v.pattern.trimmed() == item->text().trimmed()) {
      v.is_enabled = item->checkState() == Qt::Checked;
      qDebug() << v.pattern << " => " << v.is_enabled;
    }
  }
}

void FilterParamsForm::update() {
  if (m_filter_model != nullptr) {
    for (int i = 0; i < m_filter_model->rowCount(); ++i) {
      delete m_filter_model->item(i);
    }
    m_filter_model->clear();
  }
  m_filter_model = std::make_shared<QStandardItemModel>(ui->filtrListView);
  ui->filtrListView->setModel(m_filter_model.get());
  int row = 0;
  for (auto fltr : m_filters) {
    QStandardItem *item = new QStandardItem(fltr.pattern);
    item->setCheckable(true);
    auto state = fltr.is_enabled ? Qt::Checked : Qt::Unchecked;
    item->setCheckState(state);
    item->setData(state, Qt::CheckStateRole);
    m_filter_model->setItem(row++, item);
  }
  connect(m_filter_model.get(), &QStandardItemModel::itemChanged, this,
          &FilterParamsForm::fltrItemChangedSlot);
}

Filter_Ptr FilterParamsForm::Filter() const {
  auto fltr = std::make_shared<FilterUnion>(FilterUnion::UnionKind::AND);

  if (ui->timeRangeGroupBox->isChecked()) {
    auto timeFltr =
        std::make_shared<DateRangeFilter>(fromTimeEdit->time(), toTimeEdit->time());
    fltr->addFilter(timeFltr);
  }

  if (!m_filters.empty()) {
    auto combination = ui->AndRadioButton->isChecked() ? FilterUnion::UnionKind::AND
                                                       : FilterUnion::UnionKind::OR;
    qDebug() << "string filters combination: "
             << (combination == FilterUnion::UnionKind::AND ? "And" : "Or");

    auto str_fltr = std::make_shared<FilterUnion>(combination);
    for (const auto &s : m_filters) {
      qDebug() << s.pattern << " => " << s.is_enabled;
      if (s.is_enabled) {
        str_fltr->addFilter(std::make_shared<StringFilter>(s.pattern));
      }
    }
    fltr->addFilter(str_fltr);
  }

  return fltr;
}

void FilterParamsForm::addFltrSlot() {
  qDebug() << "FilterParamsForm::addFltrSlot()";
  auto text = ui->fltrEdit->text();
  if (text.trimmed() != "") {
    m_filters << StringFilterDescription{true, text.trimmed()};
    update();
  }
  ui->fltrEdit->clear();
}

void FilterParamsForm::rmSelectedFiltrSlot() {
  qDebug() << "FilterParamsForm::rmSelectedFiltrSlot()";
  auto selectedIndexes = ui->filtrListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.size() > 0) {
    m_filters.removeAt(selectedIndexes.front().row());
    update();
  }
}

void FilterParamsForm::dateRangeChangedSlot() {
  qDebug() << "FilterParamsForm::dateRangeChangedSlot()";
}

void FilterParamsForm::applyFilterSlot() {
  qDebug() << "FilterParamsForm::applyFilterSlot()";
  emit filterApply();
}
