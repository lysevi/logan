#include "log.h"
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>
//#include <QtConcurrent>
#include <QScrollBar>
#include <QTextCodec>

const QRegExp dateRegex("\\d\\d:\\d\\d:\\d\\d");

LinePositionList allLinePos(const QByteArray&bts){

    int count=0;
    auto size=bts.size();
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
        m_bts=std::move(inputFile.readAll());
        m_lines=allLinePos(m_bts);

        inputFile.close();
    }else{
        throw std::logic_error("file not exists!");
    }
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
    m_load_complete=true;
}

Log::Log(const QFileInfo& fileInfo,
         const QString&filename,
         const HighlightPatterns *global_highlight,
         QObject *parent):QAbstractItemModel(parent){

    m_fileInfo=fileInfo;
    m_lastModifed=m_fileInfo.lastModified();
    m_name=m_fileInfo.fileName();
    m_fname=filename;
    m_global_highlight=global_highlight;
    m_load_complete=false;
    loadFile();
    auto idx=createIndex(-1,-1,nullptr);
    while(canFetchMore(idx)){
        fetchMore(idx);
    }
    qDebug()<<"loaded "<<m_name<<" lines:"<<m_lines.size();
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
        auto bts=inputFile.readAll();
        auto lines=allLinePos(bts);
        auto diff=lines.size()-m_lines.size();
        if(diff>0){
            //m_load_complete=false;
            //this->beginResetModel();
            m_bts=std::move(bts);


            m_lines=std::move(lines);

            m_load_complete=true;
            //this->endResetModel();

            emit countChanged(m_lines.size());
            emit linesChanged();

           int loaded=0;
            std::map<int, CachedString> local_res;
            for(size_t i=0;i<diff;++i){
                CachedString cs;
                cs.originValue=makeString(m_cache.size()+i);
                cs.Value=cs.originValue;
                local_res.insert(std::make_pair(m_cache.size()+i, cs));
                loaded++;
            }
            beginInsertRows(createIndex(0,0,nullptr), m_cache.size(), m_cache.size()+loaded-1);
            for(auto&kv:local_res){
                m_cache.insert(std::make_pair(kv.first, kv.second));
            }
            endInsertRows();
            emit m_lv_object->scrollToBottom();
        }

        inputFile.close();
    }else{
        throw std::logic_error("file not exists!");
    }

    qDebug()<<"update elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}




std::shared_ptr<QString> Log::makeString(int row, bool isPlain)const{
    auto line_pos=m_lines[row];
    int start=line_pos.first;
    int i=line_pos.second;

#ifdef WIN32
    QTextCodec * codec = QTextCodec::codecForName( "CP1251" );
#else
    QTextCodec * codec = QTextCodec::codecForName( "UTF-8" );
#endif
    int stringSize=int(i-start+1);
    QByteArray localStr(stringSize, ' ');

    int insertPos=0;
    for(int pos=start;pos<i;++pos){
        localStr[insertPos++]=m_bts[pos];
    }

    std::shared_ptr<QString> result=std::make_shared<QString>(codec->toUnicode(localStr));

    if(!isPlain)
    {
        if(m_global_highlight==nullptr){
            throw std::logic_error("m_global_highlight==nullptr");
        }
        for(auto it=m_global_highlight->begin();it!=m_global_highlight->end();++it){
            heighlightStr(result.get(), *it);
        }
    }
    return result;
}

int Log::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    if(!m_load_complete){
        return 0;
    }
   //return m_lines.size();
    return m_cache.size();
}


QVariant Log::data(const QModelIndex & index, int role) const {

    const QString emptyString="EmptyString";
    if(!m_load_complete ){
        return emptyString;
    }

    if (index.row() < 0 || index.row() >= int(m_lines.size()))
        return QVariant();
    if (role == Qt::DisplayRole || role == Qt::EditRole){
        auto it=m_cache.find(index.row());
        if(it!=m_cache.end()){
            return *it->second.Value;
        }
    }
    return QVariant();
}

QString Log::plainText(const QModelIndex & index)const{
    if (index.row() < 0 || index.row() >= int(m_lines.size()))
        return QString("error");
    return *makeString(index.row(), true);
}

bool Log::canFetchMore(const QModelIndex &index) const {
    if (m_lines.size()==m_cache.size()){
        qDebug()<<"canFetchMore "<<index.row()<<"isvalid:"<<index.isValid()<<"false";
        return false;
    }else{
        qDebug()<<"canFetchMore "<<index.row()<<"true";
        return true;
    }
}

void Log::fetchMore(const QModelIndex &index) {
    qDebug()<<"fetchMore"<<index.row();

    const int loadStep=1000;
    int loaded=0;
    std::map<int, CachedString> local_res;
    for(int i=0;i<loadStep;++i){
        if((m_cache.size()+local_res.size())==m_lines.size()){
            break;
        }
        CachedString cs;
        cs.originValue=makeString(m_cache.size()+i);
        cs.Value=cs.originValue;
        local_res.insert(std::make_pair(m_cache.size()+i, cs));
        loaded++;
    }
    beginInsertRows(index, m_cache.size(), m_cache.size()+loaded);
    for(auto&kv:local_res){
        m_cache.insert(std::make_pair(kv.first, kv.second));
    }
    endInsertRows();

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
            str->replace(re,"<b>"+captured_str+"</b>");
        }
        result=true;
    }

    return result;
}

void Log::updateHeighlights(QVector<CachedString>::iterator /*begin*/, QVector<CachedString>::iterator /*end*/,const QString&pattern){
    if(pattern.size()==0){
        return;
    }
    auto curDT=QDateTime::currentDateTimeUtc();

    //    QtConcurrent::blockingMap(begin, end,[this,&pattern](CachedString&cs){
    //        QString str(*cs.Value);
    //        auto is_updated=heighlightStr(&str, pattern);
    //        if(is_updated){
    //            cs.Value=std::make_shared<QString>(str);
    //        }
    //    });
    //    for(int k=0;k<m_buffer.size();++k){
    //        auto mi=this->createIndex(k,0);
    //        dataChanged(mi, mi);
    //    }
    qDebug()<<m_fname<<"updateHeighlights elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Log::updateHeighlights(const QString&/*pattern*/){
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
