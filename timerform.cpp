#include "timerform.h"
#include "ui_timer_setup.h"
#include <QDebug>

const bool defaultTimerState=false;
const int defaultTimerValue=5;

TimerForm::TimerForm(QWidget *parent) : QWidget(parent), ui(new Ui::TimerForm)
{
    ui->setupUi(this);


    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &TimerForm::timerParamChangedSlot);
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &TimerForm::checkBoxStatChangedSlot);
    defaultState();
}

void TimerForm::defaultState(){
    ui->horizontalSlider->setValue(defaultTimerValue);

    ui->checkBox->setChecked(defaultTimerState);
    ui->groupBox->setEnabled(defaultTimerState);
    ui->timerValueLabel->setText(QString::number(ui->horizontalSlider->value()));

    emit timerParamChangedSlot(defaultTimerValue);
    emit timerIsEnabledSignal(defaultTimerState);
}

void TimerForm::timerParamChangedSlot(int v){
    qDebug()<<"change timer:"<<v;
    if(v>0){
        emit timerParamChangedSignal(v);
    }
}

void TimerForm::checkBoxStatChangedSlot(int v){
    bool isChecked=v==Qt::Checked;
    qDebug()<<"timer is :"<<isChecked;

    emit timerIsEnabledSignal(isChecked);
    ui->groupBox->setEnabled(isChecked);
    ui->timerValueLabel->setText(QString::number(ui->horizontalSlider->value()));

}

TimerForm::~TimerForm(){
    delete ui;
}
