#ifndef LOGLINE_H
#define LOGLINE_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QVariant>

class LogLine : public QObject
{
    Q_OBJECT
private:
    //Q_PROPERTY(QTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    //Q_PROPERTY(QString type READ type NOTIFY typeChanged)
public:
    explicit LogLine(QObject *parent = nullptr);
    LogLine(const QString&messages, QObject *parent=0);
    ~LogLine();
    //QTime timestamp() const;
    QString message() const;
    //QString type()const;
signals:
    void messageChanged();
    /*void timestampChanged();
    void countChanged();
    void typeChanged();*/
private:
    //QTime m_time;
    QString m_message;
    //QString m_type;
};
#endif // LOGLINE_H
