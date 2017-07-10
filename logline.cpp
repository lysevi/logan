#include "logline.h"
#include <QDebug>
LogLine::LogLine(QObject *parent) : QObject(parent)
{

}

LogLine::LogLine(const QTime &timestamp, const QString&typeStr,const QList<QObject*> &messages, QObject *parent):QObject(parent){
    m_time=timestamp;
    m_messages=messages;
    m_variant_message= QVariant::fromValue(m_messages);
    m_type=typeStr;
}

LogLine::~LogLine(){
    for(auto v:m_messages){
        delete v;
    }
}

QTime LogLine::timestamp() const{
    qDebug()<<"read ts";
    return m_time;
}


QList<QObject*> LogLine::messages() const{
    qDebug()<<"read messages";
    return m_messages;
}


int LogLine::count()const{
    return m_messages.size();
}

QString LogLine::type()const{
    return m_type;
}
