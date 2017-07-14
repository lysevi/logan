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
    qDebug()<<"loaded "<<m_name<<" lines:"<<m_lines.count();
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
        m_bts=inputFile.readAll();
        m_lines=allLinePos(m_bts);
        inputFile.close();
    }else{
        throw std::logic_error("file not exists!");
    }
}

void Log::update(){
    qDebug()<<"update "<<m_name;
    this->beginResetModel();
    m_line_cache.clear();
    m_lines.clear();
    loadFile();

    emit countChanged(m_lines.size());
    emit linesChanged();
    this->endResetModel();
}


int Log::rowCount(const QModelIndex & parent) const {
    qDebug()<<"rowCount";
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
        return *it->Value;
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
        cs.rawValue=std::make_shared<QString>(line);
        cs.Value=std::make_shared<QString>(line);
        heighlightStr(cs.Value.get(), m_heighlight_patterns+*m_global_highlight);
        m_line_cache.insert(index.row(), cs);
        return *cs.Value;
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
            qDebug()<<"H+"<<*str;
            auto ct=re.capturedTexts();
            for(auto&&captured_str:ct){
                qDebug()<<"cs"<<captured_str;
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
    auto keys=m_line_cache.keys();
    for(auto&k:keys){
        auto cs=m_line_cache[k];

        auto res=std::make_shared<QString>(*cs.rawValue.get());
        heighlightStr(res.get(), superSet);

        if(res!=cs.rawValue){
            cs.Value=res;
        }else{
            cs.Value=cs.rawValue;
        }
        m_line_cache.insert(k,cs);
        auto mi=this->createIndex(k,0);
        dataChanged(mi, mi);

    }
}

void Log::addHeighlightPatter(const QString&sl){
    m_heighlight_patterns<<sl;
    updateHeighlights();
}

