#include "logline.h"
#include <QDebug>
LogLine::LogLine(QObject *parent) : QObject(parent)
{

}

LogLine::LogLine(const QString &messages, QObject *parent):QObject(parent){
    //m_time=timestamp;
    m_message=messages;
    //m_type=typeStr;
}

LogLine::~LogLine(){
}

QString LogLine::message() const{
    qDebug()<<"read message";
    return m_message;
}

//QTime LogLine::timestamp() const{
//    qDebug()<<"read ts";
//    return m_time;
//}




//QString LogLine::type()const{
//    return m_type;
//}
