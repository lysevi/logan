#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>
#include "log.h"
#include "listboxeditableitem.h"

namespace Ui {
class LogViewer;
}

class LogViewer : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = 0);
    ~LogViewer();
    void setModel(Log*model_);
    Log* model()const{
        return m_model;
    }
private:
    Ui::LogViewer *ui;
    Log* m_model;
    ListboxEditableItem m_delegate;
};

#endif // LOGVIEWER_H
