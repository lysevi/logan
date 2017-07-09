#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "log.h"
#include "windowcontroller.h"

Log* createLogMock(QDateTime curDT, const QString&name){
    Log* ll;
    QList<LogLine*> loglines;
    for(int i=0;i<1000;++i){
        QList<QObject*> ql1;
        ql1<<new Message(name+ "m1"+QString::number(i))
          <<new Message(name+ "line1\nline"+QString::number(i))
         <<new Message(name+ "m111"+QString::number(i));

        loglines.append(new LogLine(curDT,ql1));
        QList<QObject*> ql2;
        ql2<<new Message(name+ "m2"+QString::number(i));
        loglines.append(new LogLine(curDT.addSecs(10),ql2));
        curDT=curDT.addSecs(i*60);
    }
    ll=new Log(name,loglines);
    return ll;
}

QList<Log*> createLogs(int count){
    QList<Log*> logs;
    auto curDT=QDateTime::currentDateTime();
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

    auto rootObject=engine.rootObjects().first();
    qDebug()<<"rootObject:"<<rootObject->objectName();
    WindowController *wc=new WindowController();
    QObject::connect(rootObject, SIGNAL(updateAllSignal(QString)),
                     wc,  SLOT(updateAllSlot(QString)));

    auto logs=createLogs(5);

    wc->addLogs(logs);

    for(auto&v:logs){
        QVariant returnedValue;
        QMetaObject::invokeMethod(rootObject, "addTab",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, v->name()), Q_ARG(QVariant, QVariant::fromValue(v)));
    }


    return app.exec();
}
