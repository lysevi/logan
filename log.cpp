#include "log.h"
#include <QDebug>
#include <QRegExp>
Log::Log(QObject *parent) : QObject(parent)
{

}

Log* Log::openFile(const QString&fname, QObject *parent){
    qDebug()<<"openFile"<<fname;
    Log* ll;
    QList<QObject*> loglines;

    QFile inputFile(fname);
    QRegExp dateRE("(^\\d\\d:\\d\\d:\\d\\d\\.\\d*)");
    QRegExp typeRE("(\\[\\w{0,3}\\])");
    QRegExp messageRE("\\[\\w{0,3}\\](.*$)");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QString typeStr;

          if(typeRE.indexIn(line)!=-1){
              typeStr=typeRE.cap(1);
              //qDebug()<<"type: "<<typeStr;
          }

          if(dateRE.indexIn(line)!=-1){
             QString dateStr=dateRE.cap(1);
             //qDebug()<<"date: "<<dateStr;

             if(messageRE.indexIn(line)!=-1){
                QString messageStr=messageRE.cap(1);
                QList<QObject*> ql1;
                ql1<<new Message(messageStr);
                loglines.append(new LogLine(QTime::fromString(dateStr), typeStr,ql1));
             }
          }
       }
       inputFile.close();
    }

    ll=new Log(fname,loglines, parent);
    return ll;
}

Log::Log(const QString&name, const QList<QObject*>&lines, QObject *parent):QObject(parent){
    qDebug()<<"load "<<name<<" lines:"<<lines.count();
    m_name=name;
    m_lines=lines;
    qDebug()<<"load "<<name<<" loaded";
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
