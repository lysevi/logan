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
    auto ll=new Log(fileInfo, fname, global_highlight, parent);
    return ll;
}

void Log::loadFile(){
    qDebug()<<"loadFile "<<m_fname;
    auto curDT=QDateTime::currentDateTimeUtc();
    QFile inputFile(m_fname);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        auto bts=inputFile.map(0, inputFile.size());
        m_bts.resize(inputFile.size());
        for(int i=0;i<m_bts.size();++i){
            m_bts[i]=bts[i];
        }
        m_lines=allLinePos(bts, inputFile.size());

        inputFile.unmap(bts);
        inputFile.close();
    }else{
        throw std::logic_error("file not exists!");
    }
    if(m_global_highlight==nullptr){
        throw std::logic_error("m_global_highlight==nullptr");
    }
    for(auto&pattern:*m_global_highlight){
        updateHeighlights(pattern);
    }
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
    m_load_complete=true;
}

Log::Log(const QFileInfo& fileInfo,
         const QString&filename,
         const HighlightPatterns *global_highlight,
         QObject *parent):QAbstractListModel(parent){

    m_fileInfo=fileInfo;
    m_lastModifed=m_fileInfo.lastModified();
    m_name=m_fileInfo.fileName();
    m_fname=filename;
    m_global_highlight=global_highlight;
    m_load_complete=false;
    loadFile();
    qDebug()<<"loaded "<<m_name<<" lines:"<<m_lines.count();
}


QString Log::name()const{
    return m_name;
}

QString Log::filename()const{
    return m_fname;
}


void Log::update(){
    qDebug()<<"update "<<m_name<<"=>"<<m_fname;
    QFileInfo fileInfo(m_fname);

    //    if(fileInfo.lastModified()<=m_lastModifed){
    //        qDebug()<<"nothing to read: curDt:"<<fileInfo.lastModified()<<" myDt:"<<m_fileInfo.lastModified();
    //        return;
    //    }
    m_lastModifed=fileInfo.lastModified();


    qDebug()<<"loadFile "<<m_fname;
    auto curDT=QDateTime::currentDateTimeUtc();
    QFile inputFile(m_fname);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        auto bts=inputFile.map(0, inputFile.size());
        auto lines=allLinePos(bts, inputFile.size());
        auto diff=lines.size()-m_lines.size();
        if(diff>0){
            m_load_complete=false;
            //this->beginResetModel();
            auto fsize=inputFile.size();
            auto bts=inputFile.map(0, fsize);
            int i=m_bts.size()-1;
            m_bts.resize(fsize);

            for(;i<fsize;++i){
                m_bts[i]=bts[i];
            }

            m_lines=std::move(lines);

            m_load_complete=true;
            //this->endResetModel();
            m_lv_object->scrollToBottom();
            emit countChanged(m_lines.size());
            emit linesChanged();
        }

        inputFile.unmap(bts);
        inputFile.close();
    }else{
        throw std::logic_error("file not exists!");
    }

    qDebug()<<"update elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());



}


int Log::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    if(!m_load_complete){
        return 0;
    }
    return m_lines.count();
}


QString Log::makeString(int row)const{
    auto line_pos=m_lines[row];
    int start=line_pos.first;
    int i=line_pos.second;

#ifdef WIN32
    QTextCodec * codec = QTextCodec::codecForName( "CP1251" );
#else
    QTextCodec * codec = QTextCodec::codecForName( "UTF-8" );
#endif
    int stringSize=int(i-start+1);
    QByteArray localStr(stringSize, '\0');

    int insertPos=0;
    for(int pos=start;pos<i;++pos){
        localStr[insertPos++]=m_bts[pos];
    }

    QString result=codec->toUnicode(localStr);

    if(m_global_highlight==nullptr){
        throw std::logic_error("m_global_highlight==nullptr");
    }
    for(auto it=m_global_highlight->begin();it!=m_global_highlight->end();++it){
        heighlightStr(&result, *it);
    }
    return result;
}

QVariant Log::data(const QModelIndex & index, int role) const {

    const QString emptyString="EmptyString";
    if(!m_load_complete ){
        return emptyString;
    }
    if (index.row() < 0 || index.row() >= m_lines.count())
        return QVariant();
    if (role == Qt::DisplayRole || role == Qt::EditRole){
        return makeString(index.row());
    }
    return QVariant();
}

void Log::clearHightlight(){
    //    m_load_complete=false;
    //    this->beginResetModel();
    //    QtConcurrent::blockingMap(m_buffer,[](CachedString&cs){
    //        cs.Value=cs.originValue;
    //    });
    //    for(int k=0;k<m_buffer.size();++k){
    //        auto mi=this->createIndex(k,0);
    //        dataChanged(mi, mi);
    //    }
    //    m_load_complete=true;
    //    this->endResetModel();
}

bool Log::heighlightStr(QString* str,const QString&pattern){
    if(pattern.size()==0){
        return false;
    }
    bool result=false;

    QRegExp re(pattern);
    if(re.indexIn(*str)!= -1){
        auto ct=re.capturedTexts();
        for(auto&&captured_str:ct){
            *str=str->replace(re,"<b>"+captured_str+"</b>");
        }
        result=true;
    }

    return result;
}

void Log::updateHeighlights(QVector<CachedString>::iterator begin, QVector<CachedString>::iterator end,const QString&pattern){
    if(pattern.size()==0){
        return;
    }
    auto curDT=QDateTime::currentDateTimeUtc();

    QtConcurrent::blockingMap(begin, end,[this,&pattern](CachedString&cs){
        QString str(*cs.Value);
        auto is_updated=heighlightStr(&str, pattern);
        if(is_updated){
            cs.Value=std::make_shared<QString>(str);
        }
    });
    //    for(int k=0;k<m_buffer.size();++k){
    //        auto mi=this->createIndex(k,0);
    //        dataChanged(mi, mi);
    //    }
    qDebug()<<m_fname<<"updateHeighlights elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Log::updateHeighlights(const QString&pattern){
    //updateHeighlights(m_buffer.begin(), m_buffer.end(), pattern);
}

void Log::localHightlightPattern(const QString&pattern){
    if(pattern.size()==0){
        return;
    }
    updateHeighlights(pattern);
}

void Log::setListVoxObject(QListView *object){
    m_lv_object=object;
}
