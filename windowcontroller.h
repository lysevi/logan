#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QList>
#include "log.h"

class WindowController : public QObject
{
    Q_OBJECT
public:
    WindowController(QObject* rootObject, QObject*parent=nullptr);
protected:
    QHash<QString, Log*> m_logs;
    QObject* m_rootObject;
    QSet<QString> m_heightlight;
private:
    void addTab(const QString&title, Log*v);
signals:

public slots:
    void updateAllSlot(const QString &msg);
    void openFileSlot(const QString &fname);
    void closeFileSlot(const QString &fname);
    void addHighlightedTextSlot(const QString &s);
    void clearHighlightedTextSlot();
private:
    HighlightPatterns m_global_highlight;
};

#endif // WINDOWCONTROLLER_H
