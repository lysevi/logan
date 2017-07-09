#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QString>


class Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
public:
    explicit Message(QObject *parent = nullptr);
    Message(const QString&msg,  QObject *parent = nullptr);

    QString message()const;
private:
    QString m_msg;
signals:
    void messageChanged();
public slots:
};

#endif // MESSAGE_H
