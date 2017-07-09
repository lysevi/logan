#include "windowcontroller.h"

WindowController::WindowController(QObject*parent):QObject(parent){

}

void WindowController::addLogs(const QList<Log*>&logs){
    for(auto&v:logs){
        m_logs<<v;
    }
}

void WindowController::updateAllSlot(const QString &/*msg*/) {
    for(auto&v:m_logs){
        v->update();
    }
}
