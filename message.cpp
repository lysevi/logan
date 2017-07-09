#include "message.h"

Message::Message(QObject *parent) : QObject(parent)
{

}


Message::Message(const QString&msg,  QObject *parent):QObject(parent){
    m_msg=msg;
    if(m_msg.contains("\n")){
        auto s=m_msg.toStdString();
        auto it=std::find(s.begin(), s.end(), '\n');
        m_shrtMsg=QString::fromStdString(s.substr(std::distance(s.begin(), it)))+"...";
    }else{
        m_shrtMsg=m_msg;
    }
}

QString Message::message()const{
    return m_msg;
}

QString Message::shortMessage()const{
    return m_shrtMsg;
}
