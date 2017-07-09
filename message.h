#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QString>


class Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QString shortMessage READ shortMessage NOTIFY shortMessageChanged)
public:
    explicit Message(QObject *parent = nullptr);
    Message(const QString&msg,  QObject *parent = nullptr);

    QString message()const;
    QString shortMessage()const;
private:
    QString m_msg;
    QString m_shrtMsg;
signals:
    void messageChanged();
    void shortMessageChanged();
public slots:
};

#endif // MESSAGE_H
