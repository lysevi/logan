#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "log.h"
#include "windowcontroller.h"

Log* createLogMock(QTime curDT, const QString&name){
    Log* ll;
    QList<QObject*> loglines;
    for(int i=0;i<1000;++i){
        QList<QObject*> ql1;
        ql1<<new Message(name+ "m1"+QString::number(i))
          <<new Message(name+ "line1\nline"+QString::number(i))
         <<new Message(name+ "m111"+QString::number(i));

        loglines.append(new LogLine(curDT, "null!",ql1));
        QList<QObject*> ql2;
        ql2<<new Message(name+ "m2"+QString::number(i));
        loglines.append(new LogLine(curDT.addSecs(10),"null!",ql2));
        curDT=curDT.addSecs(i*60);
    }
    ll=new Log(name,loglines);
    return ll;
}

QList<Log*> createLogs(int count){
    QList<Log*> logs;
    auto curDT=QTime::currentTime();
    for(int i=0;i<count;++i){
        logs<<createLogMock(curDT, "mockLokg" + QString::number(i));
    }
    return logs;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QQmlApplicationEngine engine;
    //    auto ctx=engine.rootContext();
    //    ctx->setContextProperty("logfile", QVariant::fromValue(ll));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject* rootObject=engine.rootObjects().first();
    qDebug()<<"rootObject:"<<rootObject->objectName();
    WindowController *wc=new WindowController(rootObject);
    QObject::connect(rootObject, SIGNAL(updateAllSignal(QString)),
                     wc,  SLOT(updateAllSlot(QString)));
    QObject::connect(rootObject, SIGNAL(openFileSignal(QString)),
                     wc,  SLOT(openFileSlot(QString)));

//    auto logs=createLogs(5);

//    wc->addLogs(logs);

    return app.exec();
}
