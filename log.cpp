#include "log.h"
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>
#include <QtConcurrent>

const QRegExp dateRegex("\\d\\d:\\d\\d:\\d\\d");

Log::Log(QObject *parent) : QAbstractListModel(parent)
{}

LinePositionList allLinePos(const uchar*bts, qint64 size){

    int count=0;
    for(int i=0;i<size;++i){
        if(bts[i]=='\n'){
            count++;
        }
    }
    LinePositionList result(count);
    int start=0;
    int index=0;
    for(int i=0;i<size;++i){
        if(bts[i]=='\n'){
            LinePosition lp;
            lp.first=start;
            lp.second=i;
            lp.index=index++;
            result[lp.index]=lp;
            start=i+1;
        }
    }
    return result;
}

Log* Log::openFile(const QString&fname, const HighlightPatterns *global_highlight,QObject *parent){
    qDebug()<<"openFile"<<fname;
    QFileInfo fileInfo(fname);
    auto ll=new Log(fileInfo.fileName(), fname, global_highlight, parent);
    return ll;
}

Log::Log(const QString&name,
         const QString&filename,
         const HighlightPatterns *global_highlight,
         QObject *parent):QAbstractListModel(parent){

    m_name=name;
    m_fname=filename;
    m_global_highlight=global_highlight;
    loadFile();
    qDebug()<<"loaded "<<m_name<<" lines:"<<m_buffer.count();
}


QString Log::name()const{
    return m_name;
}

QString Log::filename()const{
    return m_fname;
}

void Log::loadFile(){
    qDebug()<<"loadFile "<<m_fname;
    auto curDT=QDateTime::currentDateTimeUtc();
    QFile inputFile(m_fname);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        auto bts=inputFile.map(0, inputFile.size());
        auto lines=allLinePos(bts, inputFile.size());
        inputFile.close();
        initBuffer(bts, lines);
        inputFile.unmap(bts);
    }else{
        throw std::logic_error("file not exists!");
    }
    if(m_global_highlight==nullptr){
        throw std::logic_error("m_global_highlight==nullptr");
    }
    for(auto&pattern:*m_global_highlight){
        addHeighlightPattern(pattern);
    }
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Log::update(){
    qDebug()<<"update "<<m_name;
    this->beginResetModel();
    m_buffer.clear();
    loadFile();

    emit countChanged(m_buffer.size());
    emit linesChanged();
    this->endResetModel();
}


int Log::rowCount(const QModelIndex & parent) const {
    //qDebug()<<"rowCount";
    Q_UNUSED(parent);
    if(!m_load_complete){
        return 0;
    }
    return m_buffer.count();
}

QHash<int, QByteArray> Log::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "message";
    return roles;
}

void Log::initBuffer(const uchar*bts, const LinePositionList&lines){
    m_buffer.resize(lines.size());
    auto res=QtConcurrent::map(lines,[this,bts](const LinePosition&it){
        int start=it.first;
        int i=it.second;

        CachedString cs;
        cs.index=it.index;
        cs.Value.resize(int(i-start));
        int insertPos=0;
        for(int pos=start;pos<i;++pos){
            cs.Value[insertPos++]=bts[pos];
        }
        if(m_global_highlight==nullptr){
            throw std::logic_error("m_global_highlight==nullptr");
        }
        for(auto it=m_global_highlight->begin();it!=m_global_highlight->end();++it){
            heighlightStr(&cs.Value, *it);
        }
        m_buffer[it.index]=cs;
    });
    res.waitForFinished();
    m_load_complete=true;
}

QVariant Log::data(const QModelIndex & index, int role) const {
    const QString emptyString="EmptyString";
    if(!m_load_complete ){
        return emptyString;
    }
    if (index.row() < 0 || index.row() >= m_buffer.count())
        return QVariant();
    if (role == MessageRole){
        return (m_buffer[index.row()].Value);
    }
    return QVariant();
}

void Log::clearHeightlight(){
    m_load_complete=false;
    update();
}

bool Log::heighlightStr(QString* str,const QString&pattern){
    bool result=false;

    QRegExp re(pattern);
    if(re.indexIn(*str)!= -1){
        auto ct=re.capturedTexts();
        for(auto&&captured_str:ct){
            str->replace(re,"<b>"+captured_str+"</b>");
        }
        result=true;
    }

    return result;
}

void Log::updateHeighlights(const QString&pattern){
    auto curDT=QDateTime::currentDateTimeUtc();

    QtConcurrent::blockingMap(m_buffer,[this,&pattern](CachedString&cs){
        heighlightStr(&cs.Value, pattern);

    });
    for(int k=0;k<m_buffer.size();++k){
        auto mi=this->createIndex(k,0);
        dataChanged(mi, mi);
    }
    qDebug()<<m_fname<<"updateHeighlights elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Log::addHeighlightPattern(const QString&pattern){
    updateHeighlights(pattern);
}

