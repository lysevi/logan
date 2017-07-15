#include "log.h"
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>


const QRegExp dateRegex("\\d\\d:\\d\\d:\\d\\d");

Log::Log(QObject *parent) : QAbstractListModel(parent)
{}

LinePositionList allLinePos(const QByteArray&bts){

    LinePositionList result;
    int count=0;
    for(int i=0;i<bts.size();++i){
        if(bts[i]=='\n'){
            count++;
        }
    }
    result.reserve(count);
    int start=0;
    for(int i=0;i<bts.size();++i){
        if(bts[i]=='\n'){
            result.append(QPair<int,int>(start,i));
            start=i+1;
        }
    }
    return result;
}

Log* Log::openFile(HighlightPatterns *global_highlight, const QString&fname, QObject *parent){
    qDebug()<<"openFile"<<fname;
    QFileInfo fileInfo(fname);
    auto ll=new Log(fileInfo.fileName(), fname, global_highlight, parent);
    return ll;
}

Log::Log(const QString&name,
         const QString&filename,
         HighlightPatterns *global_highlight,
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
        auto bts=inputFile.readAll();
        auto lines=allLinePos(bts);
        inputFile.close();
        initBuffer(bts, lines);
    }else{
        throw std::logic_error("file not exists!");
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
    return m_buffer.count();
}

QHash<int, QByteArray> Log::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "message";
    return roles;
}

void Log::readStrings(int begin, int end,const LinePositionList&lines, const QByteArray&bts){
    qDebug()<<"readStrings: begin="<<begin<<"end="<<end;
    auto diff=end-begin;
    if(diff<=100000){
        int index=begin;
        for(auto read_pos=begin;read_pos!=end;++read_pos){
            auto it=lines[read_pos];
            int start=it.first;
            int i=it.second;

            QString line(int(i-start));
            int insertPos=0;
            for(int pos=start;pos<i;++pos){
                line[insertPos++]=bts[pos];
            }
            CachedString cs;
            cs.rawValue=std::make_shared<QString>(line);
            cs.Value=std::make_shared<QString>(line);
            heighlightStr(cs.Value.get(), m_heighlight_patterns+*m_global_highlight);
            m_buffer[index]=cs;
            ++index;
        }
        return;
    }

    auto mid=begin+diff/2;
    auto handle1= std::async(std::launch::async,Log::readStrings,this, begin, mid,lines, bts);
    auto handle2= std::async(std::launch::async,Log::readStrings,this, mid, end,lines,bts);

    handle1.wait();
    handle2.wait();
}

void Log::initBuffer(const QByteArray&bts, const LinePositionList&lines){
    m_buffer.resize(lines.size());
    readStrings(0, lines.size(), lines, bts);
    //    for(const auto&pos:lines){
    //        int start=pos.first;
    //        int i=pos.second;

    //        QString line(int(i-start));
    //        int insertPos=0;
    //        for(int pos=start;pos<i;++pos){
    //            line[insertPos++]=bts[pos];
    //        }
    //        CachedString cs;
    //        cs.rawValue=std::make_shared<QString>(line);
    //        cs.Value=std::make_shared<QString>(line);
    //        heighlightStr(cs.Value.get(), m_heighlight_patterns+*m_global_highlight);
    //        m_buffer[index]=cs;
    //        ++index;
    //    }
}

QVariant Log::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_buffer.count())
        return QVariant();
    if (role == MessageRole){
        return *(m_buffer[index.row()].Value);
    }
    return QVariant();
}

void Log::clearHeightlight(){
    m_heighlight_patterns.clear();
    updateHeighlights();
}

bool Log::heighlightStr(QString* str,const HighlightPatterns&sl){
    bool result=false;
    for(auto&hWord:sl){
        QRegExp re(hWord);
        if(re.indexIn(*str)!= -1){
            //qDebug()<<"H+"<<*str;
            auto ct=re.capturedTexts();
            for(auto&&captured_str:ct){
                //qDebug()<<"cs"<<captured_str;
                str->replace(re,"<b>"+captured_str+"</b>");
            }
            result=true;
        }
    }
    return result;
}

void Log::updateHeighlights(){
    HighlightPatterns superSet(m_heighlight_patterns);
    superSet+=*m_global_highlight;

    for(int k=0;k<m_buffer.size();++k){
        auto cs=m_buffer[k];

        auto res=std::make_shared<QString>(*cs.rawValue.get());
        heighlightStr(res.get(), superSet);

        if(res!=cs.rawValue){
            cs.Value=res;
        }else{
            cs.Value=cs.rawValue;
        }
        m_buffer.insert(k,cs);
        auto mi=this->createIndex(k,0);
        dataChanged(mi, mi);

    }
}

void Log::addHeighlightPatter(const QString&sl){
    m_heighlight_patterns<<sl;
    updateHeighlights();
}

