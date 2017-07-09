#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "log.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    auto curDT=QDateTime::currentDateTime();

    QList<LogLine*> loglines;
    for(int i=0;i<50000;++i){
        QList<QObject*> ql1;
        ql1<<new Message("m1"+QString::number(i))
          <<new Message("line1\nline"+QString::number(i))
         <<new Message("m111"+QString::number(i));

        loglines.append(new LogLine(curDT,ql1));
        QList<QObject*> ql2;
        ql2<<new Message("m2"+QString::number(i));
        loglines.append(new LogLine(curDT.addSecs(10),ql2));
        curDT=curDT.addSecs(i*60);
    }
    Log *ll=new Log(loglines);
    QQmlApplicationEngine engine;
    auto ctx=engine.rootContext();
    ctx->setContextProperty("logfile", QVariant::fromValue(ll));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
