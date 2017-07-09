#include "message.h"

Message::Message(QObject *parent) : QObject(parent)
{

}


Message::Message(const QString&msg,  QObject *parent):QObject(parent){
    m_msg=msg;
}

QString Message::message()const{
    return m_msg;
}
