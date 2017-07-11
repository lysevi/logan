#include "windowcontroller.h"

WindowController::WindowController(QObject* rootObject,QObject*parent):QObject(parent){
    m_rootObject=rootObject;
}

void WindowController::addTab(const QString&title, Log*v){
    QVariant returnedValue;
    QMetaObject::invokeMethod(m_rootObject, "addTab",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, title), Q_ARG(QVariant, QVariant::fromValue(v)));
}

void WindowController::updateAllSlot(const QString &/*msg*/) {
    for(auto&v:m_logs){
        v->update();
    }
}


void WindowController::openFileSlot(const QString &fname){
    if(m_logs.contains(fname)){
        return;
    }
    auto log=Log::openFile(fname);
    log->heighlightWords(m_heightlight);
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
    m_heightlight<<s;
    for(auto&kv:m_logs){
        kv->heighlightWords(m_heightlight);
    }
}

void WindowController::clearHighlightedTextSlot(){
    qDebug()<<"clearHighlightedTextSlot";
    m_heightlight.clear();
    for(auto&kv:m_logs){
        kv->clearHeightlight();
    }
}
