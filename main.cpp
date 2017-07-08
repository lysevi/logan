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

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QQmlContext *ctxt = view.rootContext();
    ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));
    view.setSource(QUrl("qrc:/main.qml"));

    return app.exec();
}
