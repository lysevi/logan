#ifndef LOG_H
#define LOG_H

#include <QObject>
#include "logline.h"


class Log : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> lines READ lines  NOTIFY linesChanged)
    Q_PROPERTY(int count READ count  NOTIFY countChanged)
    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
public:
    explicit Log(QObject *parent = nullptr);
    Log(const QString&name,const QList<LogLine*>&lines, QObject *parent = nullptr);

    QList<QObject*> lines()const;
    int count()const;
    QString name()const;
signals:
    void linesChanged();
    void countChanged();
    void nameChanged();
public slots:

protected:
    QList<QObject*> m_lines;
    QString m_name;
};

#endif // LOG_H
