#include "windowcontroller.h"
#include <QtConcurrent>
#include <QDateTime>

WindowController::WindowController(QObject* rootObject,QObject*parent):QObject(parent){
    auto threads=QThreadPool::globalInstance()->maxThreadCount();
    //QThreadPool::globalInstance()->setMaxThreadCount(threads);
    qDebug()<<"maxThreadCount"<<threads;
    m_rootObject=rootObject;
    clearHighlightedTextSlot();
}

void WindowController::addTab(const QString&title, Log*v){
    QVariant returnedValue;
    QMetaObject::invokeMethod(m_rootObject, "addTab",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, title), Q_ARG(QVariant, QVariant::fromValue(v)));
}

void WindowController::updateAllSlot(const QString &msg) {
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


void WindowController::openFileSlot(const QString &fname){
    if(m_logs.contains(fname)){
        return;
    }
    auto log=Log::openFile(fname, &m_global_highlight);
    m_logs[fname]=log;
    addTab(fname, log);
}


void WindowController::closeFileSlot(const QString &fname){
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

void WindowController::addHighlightedTextSlot(const QString &s){
    qDebug()<<"addHighlightedTextSlot "<<s;
    auto curDT=QDateTime::currentDateTimeUtc();
    m_global_highlight<<s;
    QtConcurrent::blockingMap(m_logs,[&s](auto v){v->updateHeighlights(s);});
    qDebug()<<"elapsed time:"<< curDT.secsTo(QDateTime::currentDateTimeUtc());
}

void WindowController::clearHighlightedTextSlot(){
    qDebug()<<"clearHighlightedTextSlot";
    m_global_highlight.clear();
    m_global_highlight.insert(dateRe);
    for(auto log:m_logs){
        log->clearHightlight();
    }
}
