#include "windowcontroller.h"

WindowController::WindowController(QObject* rootObject,QObject*parent):QObject(parent){
    m_rootObject=rootObject;
}

void WindowController::addLogs(const QList<Log*>&logs){
    for(auto&v:logs){
        m_logs<<v;
        addTab(v->name(),v);
    }
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
   auto log=Log::openFile(fname);
   m_logs.append(log);
   addTab(fname, log);
}
