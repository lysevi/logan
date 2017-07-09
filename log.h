#ifndef LOG_H
#define LOG_H

#include <QObject>
#include "logline.h"


class Log : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> lines READ lines  NOTIFY linesChanged)
    Q_PROPERTY(int count READ count  NOTIFY countChanged)
public:
    explicit Log(QObject *parent = nullptr);
    Log(const QList<LogLine*>&lines, QObject *parent = nullptr);

    QList<QObject*> lines()const;
    int count()const;
signals:
    void linesChanged();
    void countChanged();
public slots:

protected:
    QList<QObject*> m_lines;
};

#endif // LOG_H
