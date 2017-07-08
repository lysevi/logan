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
    for(int i=0;i<100;++i){
        dataList.append(new LogLine(curDT, QStringList({"m1"+QString::number(i), "m11"+QString::number(i), "m111"+QString::number(i)})));
        dataList.append(new LogLine(curDT.addSecs(10), QStringList({"m2"+QString::number(i)})));
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
