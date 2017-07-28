#ifndef TIMEEDITFORM_H
#define TIMEEDITFORM_H

#include <QWidget>

namespace Ui {
class TimeEditForm;
}

enum class TIME_RANGE { MIN, MAX };

class TimeEditForm : public QWidget {
  Q_OBJECT

public:
  explicit TimeEditForm(TIME_RANGE tr, QWidget *parent = 0);
  ~TimeEditForm();

  QTime time() const;

protected:
  void updateTimeEditFromSlider();
signals:
  void timeChanged();
public slots:
  void sliderChangedSlot();
  void timeChangedSlot();
  void setStartValuesSlot();

private:
  Ui::TimeEditForm *ui;
  TIME_RANGE _tr;
};

#endif // TIMEEDITFORM_H
