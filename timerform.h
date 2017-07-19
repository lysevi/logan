#ifndef TIMERFORM_H
#define TIMERFORM_H

#include <QWidget>
namespace Ui {
class TimerForm;
}
class TimerForm : public QWidget
{
    Q_OBJECT
public:
    explicit TimerForm(QWidget *parent = nullptr);
    void defaultState();
    ~TimerForm();
    Ui::TimerForm *ui;
signals:
    void timerParamChangedSignal(int);
    void timerIsEnabledSignal(bool);
public slots:
    void timerParamChangedSlot(int);
    void checkBoxStatChangedSlot(int);
};

#endif // TIMERFORM_H
