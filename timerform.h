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

    Ui::TimerForm *ui;
signals:

public slots:
};

#endif // TIMERFORM_H
