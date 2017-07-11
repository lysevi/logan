#include "log.h"
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>
Log::Log(QObject *parent) : QAbstractListModel(parent)
{

}

QList<QPair<int,int>> allLinePos(const QByteArray&bts){
    auto curDT=QDateTime::currentDateTimeUtc();
    QList<QPair<int,int>> result;
    int start=0;
    for(int i=0;i<bts.size();++i){
        if(bts[i]=='\n'){
            result.append(QPair<int,int>(start,i));
            start=i+1;
        }
    }
    qDebug()<<"allLinePos elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
    return result;
}

Log* Log::openFile(const QString&fname, QObject *parent){
    qDebug()<<"openFile"<<fname;
    auto curDT=QDateTime::currentDateTimeUtc();
    Log* ll;
    QFile inputFile(fname);
    //QRegExp lineRe("(^\\d\\d:\\d\\d:\\d\\d\\.\\d*)\\s(\\[\\w{0,3}\\])(.*)");
    if (inputFile.open(QIODevice::ReadOnly))
    {
        auto all_bts=inputFile.readAll();
        auto allLinePositions=allLinePos(all_bts);
        inputFile.close();


        QFileInfo fileInfo(fname);
        ll=new Log(fileInfo.fileName(),all_bts, allLinePositions, parent);
        qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
        return ll;
    }
    throw std::logic_error("file not exists!");
}

Log::Log(const QString&name, const QByteArray&bts,const LinePositionList&lines, QObject *parent):QAbstractListModel(parent){
    qDebug()<<"load "<<name<<" lines:"<<lines.count();
    m_name=name;
    m_lines=lines;
    m_bts=bts;
    qDebug()<<"load "<<name<<" loaded";
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


int Log::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_lines.count();
}

QHash<int, QByteArray> Log::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "message";
    return roles;
}

QVariant Log::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_lines.count())
        return QVariant();

    auto pos = m_lines[index.row()];
    if (role == MessageRole){
        //TODO cache!
        int start=pos.first;
        int i=pos.second;
        QString line(int(i-start));
        int insertPos=0;
        for(int pos=start;pos<i;++pos){
            line[insertPos++]=m_bts[pos];
        }
        return line;
    }
    return QVariant();
}
