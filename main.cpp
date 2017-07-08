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
    dataList.append(new LogLine(QDateTime::currentDateTime(), "m1"));
    dataList.append(new LogLine(QDateTime::currentDateTimeUtc(), "m2"));

    QQmlApplicationEngine engine;
    auto ctx=engine.rootContext();
    ctx->setContextProperty("myModel", QVariant::fromValue(dataList));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
