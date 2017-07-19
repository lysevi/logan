#include "timerform.h"
#include "ui_timer_setup.h"

TimerForm::TimerForm(QWidget *parent) : QWidget(parent), ui(new Ui::TimerForm)
{
    ui->setupUi(this);
    ui->horizontalSlider->setValue(5);
}
