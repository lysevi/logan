#include "log.h"

Log::Log(QObject *parent) : QObject(parent)
{

}

Log::Log(const QList<LogLine*>&lines, QObject *parent):QObject(parent){
    for(auto v:lines){
        m_lines<<v;
    }
}


QList<QObject*> Log::lines()const{
    return m_lines;
}

int Log::count()const{
    return m_lines.size();
}
