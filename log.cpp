#include "log.h"
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>
Log::Log(QObject *parent) : QAbstractListModel(parent)
{}

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

Log* Log::openFile(HighlightPatterns *global_highlight, const QString&fname, QObject *parent){
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
        ll=new Log(fileInfo.fileName(), fname,all_bts, allLinePositions, global_highlight, parent);
        qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
        return ll;
    }
    throw std::logic_error("file not exists!");
}

Log::Log(const QString&name,
         const QString&filename,
         const QByteArray&bts,
         const LinePositionList&lines,
         HighlightPatterns *global_highlight,
         QObject *parent):QAbstractListModel(parent){
    qDebug()<<"load "<<name<<" lines:"<<lines.count();
    m_name=name;
    m_lines=lines;
    m_bts=bts;
    m_fname=filename;
    m_global_highlight=global_highlight;
    qDebug()<<"load "<<name<<" loaded";
}


QString Log::name()const{
    return m_name;
}

QString Log::filename()const{
    return m_fname;
}

void Log::update(){
    qDebug()<<"update "<<m_name;
    m_line_cache.clear();
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

    auto it=m_line_cache.find(index.row());
    if(it!=m_line_cache.end()){
        return it->Value;
    }
    auto pos = m_lines[index.row()];
    if (role == MessageRole){
        int start=pos.first;
        int i=pos.second;

        QString line(int(i-start));
        int insertPos=0;
        for(int pos=start;pos<i;++pos){
            line[insertPos++]=m_bts[pos];
        }
        CachedString cs;
        cs.rawValue=line;
        cs.Value=line;
        heighlightStr(cs.Value, m_heighlight_patterns+*m_global_highlight);
        cs.mi=index;
        m_line_cache.insert(index.row(), cs);

        return cs.Value;
    }
    return QVariant();
}

void Log::clearHeightlight(){
    m_heighlight_patterns.clear();
    updateHeighlights();
}

bool Log::heighlightStr(QString&str,const HighlightPatterns&sl){
    bool result=false;
    for(auto&hWord:sl){
        if(str.contains(hWord)){
            qDebug()<<"H+"<<str;
            str.replace(QRegExp(hWord),"<b>"+hWord+"</b>");
            result=true;
        }
    }
    return result;
}

void Log::updateHeighlights(){
    HighlightPatterns superSet(m_heighlight_patterns);
    superSet+=*m_global_highlight;
    auto keys=m_line_cache.keys();
    for(auto&k:keys){
        auto cs=m_line_cache[k];

        auto str=cs.rawValue;
        heighlightStr(str, superSet);

        cs.Value=str;
        m_line_cache.insert(k,cs);
        dataChanged(cs.mi, cs.mi);

    }
}

void Log::addHeighlightPatter(const QString&sl){
    m_heighlight_patterns<<sl;
    updateHeighlights();
}

