#include "logline.h"

LogLine::LogLine(QObject *parent) : QObject(parent)
{

}

LogLine::LogLine(const QDateTime &timestamp, const QString &message, QObject *parent):QObject(parent){
    m_time=timestamp;
    m_message=message;
}

QDateTime LogLine::timestamp() const{
    return m_time;
}
void LogLine::setTimestamp(const QDateTime &ts){
    if (m_time != ts) {
        m_time=ts;
        emit timestampChanged();
    }
}


QString LogLine::message() const{
    return m_message;
}

void LogLine::setMessage(const QString &s){
    if (m_message != s) {
        m_message=s;
        emit messageChanged();
    }
}
