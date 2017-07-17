#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QObject>
#include "log.h"
#include "windowcontroller.h"


int main(int argc, char *argv[])
{
    auto v1=QString(LVIEW_VERSION);
    auto v2=QString(GIT_VERSION);
    auto lview_version=v1+"-"+v2;

    QGuiApplication app(argc, argv);

    QtWebEngine::initialize();

    QQmlApplicationEngine engine;
    //    auto ctx=engine.rootContext();
    //    ctx->setContextProperty("logfile", QVariant::fromValue(ll));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    auto ctx=engine.rootContext();
    ctx->setContextProperty("lview_version", QVariant::fromValue(lview_version));


    QObject* rootObject=engine.rootObjects().first();
    qDebug()<<"rootObject:"<<rootObject->objectName();
    WindowController *wc=new WindowController(rootObject);

    QObject::connect(rootObject, SIGNAL(updateAllSignal(QString)),
                     wc,  SLOT(updateAllSlot(QString)));
    QObject::connect(rootObject, SIGNAL(openFileSignal(QString)),
                     wc,  SLOT(openFileSlot(QString)));
    QObject::connect(rootObject, SIGNAL(closeFileSignal(QString)),
                     wc,  SLOT(closeFileSlot(QString)));
    QObject::connect(rootObject, SIGNAL(addHighlightedTextSignal(QString)),
                     wc,  SLOT(addHighlightedTextSlot(QString)));
    QObject::connect(rootObject, SIGNAL(clearHighlightedTextSignal()),
                     wc,  SLOT(clearHighlightedTextSlot()));
    return app.exec();
}
