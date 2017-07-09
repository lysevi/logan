#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QList>

#include "log.h"

class WindowController : public QObject
{
    Q_OBJECT
public:
    explicit WindowController(QObject*parent=nullptr);

    void addLogs(const QList<Log*>&logs);
protected:
    QList<Log*> m_logs;
signals:

public slots:
    void updateAllSlot(const QString &msg);
};

#endif // WINDOWCONTROLLER_H
