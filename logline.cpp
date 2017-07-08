#include "logline.h"
#include <QDebug>
LogLine::LogLine(QObject *parent) : QObject(parent)
{

}

LogLine::LogLine(const QDateTime &timestamp, const QStringList &message, QObject *parent):QObject(parent){
    m_time=timestamp;
    m_message=message;
    m_variant_message= QVariant::fromValue(m_message);
}

QDateTime LogLine::timestamp() const{
    qDebug()<<"read ts";
    return m_time;
}


QVariant LogLine::message() const{
    qDebug()<<"read messages";
    return m_variant_message;
}
