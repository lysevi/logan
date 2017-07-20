#include "controller.h"
//#include <QtConcurrent>
#include <QDateTime>

Controller::Controller(QObject*parent):QObject(parent){
    //auto threads=QThreadPool::globalInstance()->maxThreadCount();
    //QThreadPool::globalInstance()->setMaxThreadCount(threads);
    //qDebug()<<"maxThreadCount"<<threads;
    clearHighlightedTextSlot();
}


void Controller::updateAllSlot(const QString &msg) {
    for(auto&v:m_logs){
        if(msg=="null"){
            v->update();
        }else{
            if(msg!="null" && msg==v->filename()){
                v->update();
            }
        }
    }
}


Log* Controller::openFile(const QString &fname){
    if(m_logs.contains(fname)){
        return nullptr;
    }
    auto log=Log::openFile(fname, &m_global_highlight, this);
    m_logs[fname]=log;
    return log;
}


void Controller::closeFileSlot(const QString &fname){
    auto it=m_logs.find(fname);

    if((*it)->filename()==fname){
        auto ptr=*it;
        qDebug()<<"erase "<<fname;
        delete ptr;
        m_logs.erase(it);
    }else{
        throw std::logic_error("fname not found");
    }

}

void Controller::addHighlightedTextSlot(const QString &s){
    qDebug()<<"addHighlightedTextSlot "<<s;
    auto curDT=QDateTime::currentDateTimeUtc();
    m_global_highlight<<s;
    //QtConcurrent::blockingMap(m_logs,[&s](auto v){v->updateHeighlights(s);});
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void Controller::clearHighlightedTextSlot(){
    qDebug()<<"clearHighlightedTextSlot";
    m_global_highlight.clear();
    m_global_highlight.insert(dateRe);
    for(auto log:m_logs){
        log->clearHightlight();
    }
}

