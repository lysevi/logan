#include "log.h"
#include <QDebug>
Log::Log(QObject *parent) : QObject(parent)
{

}

Log::Log(const QString&name, const QList<LogLine*>&lines, QObject *parent):QObject(parent){
    qDebug()<<"load "<<name<<" lines:"<<lines.count();
    m_name=name;
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

QString Log::name()const{
    return m_name;
}

void Log::update(){
    qDebug()<<"update "<<m_name;
    m_lines.clear();
    emit countChanged(m_lines.size());
    emit linesChanged();
}
