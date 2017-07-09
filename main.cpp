#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "logline.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QList<QObject*> dataList;
    auto curDT=QDateTime::currentDateTime();
    for(int i=0;i<50000;++i){
        QList<QObject*> ql1;
        ql1<<new Message("m1"+QString::number(i))
          <<new Message("m11"+QString::number(i))
         <<new Message("m111"+QString::number(i));

        dataList.append(new LogLine(curDT,ql1));
        QList<QObject*> ql2;
        ql2<<new Message("m2"+QString::number(i));
        dataList.append(new LogLine(curDT.addSecs(10),ql2));
        curDT=curDT.addSecs(i*60);
    }
    QQmlApplicationEngine engine;
    auto ctx=engine.rootContext();
    ctx->setContextProperty("myModel", QVariant::fromValue(dataList));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
