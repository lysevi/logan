#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "log.h"
#include "windowcontroller.h"


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
    QObject::connect(rootObject, SIGNAL(closeFileSignal(QString)),
                     wc,  SLOT(closeFileSlot(QString)));

    return app.exec();
}
