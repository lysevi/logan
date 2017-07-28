#include "timeeditform.h"
#include "ui_timeeditform.h"
#include <QDebug>
#include <QTimer>
const int MIN_SLIDER_VALUE = 1;
const int MAX_SLIDER_VALUE = 24 * 60 * 60; // secs in day

TimeEditForm::TimeEditForm(TIME_RANGE tr, QWidget *parent)
    : QWidget(parent), ui(new Ui::TimeEditForm) {
  _tr = tr;
  ui->setupUi(this);
  ui->horizontalSlider->setMinimum(MIN_SLIDER_VALUE);
  ui->horizontalSlider->setMaximum(MAX_SLIDER_VALUE);

  connect(ui->horizontalSlider, &QSlider::valueChanged, this,
          &TimeEditForm::sliderChangedSlot);
  connect(ui->timeEdit, &QTimeEdit::timeChanged, this, &TimeEditForm::timeChanged);

  QTimer::singleShot(100, this, SLOT(setStartValuesSlot()));
}

TimeEditForm::~TimeEditForm() {
  delete ui;
}

void TimeEditForm::setStartValuesSlot() {
  if (_tr == TIME_RANGE::MIN) {
    ui->horizontalSlider->setValue(MIN_SLIDER_VALUE);
  } else {
    ui->horizontalSlider->setValue(MAX_SLIDER_VALUE);
  }
  updateTimeEditFromSlider();
}

QTime TimeEditForm::time() const {
  return ui->timeEdit->time();
}

void TimeEditForm::sliderChangedSlot() {
  auto v = ui->horizontalSlider->value();
  qDebug() << "TimeEditForm::sliderChangedSlot" << v;
  updateTimeEditFromSlider();
  emit timeChanged();
}

void TimeEditForm::timeChangedSlot() {
  qDebug() << "TimeEditForm::timeChangedSlot";
  ui->horizontalSlider->setValue(ui->timeEdit->time().msecsSinceStartOfDay() / 1000);
  emit timeChanged();
}

void TimeEditForm::updateTimeEditFromSlider() {
  auto v = ui->horizontalSlider->value();
  ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(v *1000-1));
  qDebug() << "TimeEditForm::updateTimeEditFromSlider" << v << "=>"
           << QTime::fromMSecsSinceStartOfDay(v) << time();
}
