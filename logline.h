#ifndef LOGLINE_H
#define LOGLINE_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QVariant>

#include "message.h"

class LogLine : public QObject
{
    Q_OBJECT
private:
    Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(QList<QObject*> messages READ messages NOTIFY messagesChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit LogLine(QObject *parent = nullptr);
    LogLine(const QDateTime &timestamp, const QList<QObject*> &messages, QObject *parent=0);
    ~LogLine();
    QDateTime timestamp() const;
    void setTimestamp(const QDateTime &ts);

    QList<QObject*> messages() const;

    int count()const;

    QString type()const;
signals:
    void timestampChanged();
    void messagesChanged();
    void countChanged();
    void typeChanged();
private:
    QDateTime m_time;
    QList<QObject*> m_messages;
    QVariant m_variant_message;
    QString m_type;
};
#endif // LOGLINE_H
