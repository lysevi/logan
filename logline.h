#ifndef LOGLINE_H
#define LOGLINE_H

#include <QObject>
#include <QDateTime>
#include <QString>

class LogLine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
public:
    explicit LogLine(QObject *parent = nullptr);
    LogLine(const QDateTime &timestamp, const QString &message, QObject *parent=0);

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime &ts);

    QString message() const;
    void setMessage(const QString &s);
signals:
    void timestampChanged();
    void messageChanged();
private:
    QDateTime m_time;
    QString m_message;
};

#endif // LOGLINE_H
