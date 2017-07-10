#include "log.h"
#include <QDebug>
#include <QRegExp>
Log::Log(QObject *parent) : QObject(parent)
{

}

Log* Log::openFile(const QString&fname, QObject *parent){
    qDebug()<<"openFile"<<fname;
    auto curDT=QDateTime::currentDateTimeUtc();
    Log* ll;
    QList<QObject*> loglines;

    QFile inputFile(fname);
    QRegExp lineRe("(^\\d\\d:\\d\\d:\\d\\d\\.\\d*)\\s(\\[\\w{0,3}\\])(.*)");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       auto all_bts=inputFile.readAll();
       auto all_text=QString(all_bts);
       auto all_lines=all_text.split("\r\n");
       for(auto&line:all_lines){
          //qDebug()<<"line:"<<line;
          /*if(lineRe.indexIn(line)!=-1){
              QString typeStr;
              QString dateStr;

              dateStr=lineRe.cap(1);
              typeStr=lineRe.cap(2);
              QString messageStr=lineRe.cap(3);*/
              loglines.append(new LogLine(/*QTime::fromString(dateStr) typeStr,*/line));
          //}
       }
       inputFile.close();
    }

    ll=new Log(fname,loglines, parent);
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
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
