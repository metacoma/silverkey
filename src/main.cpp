#ifdef Q_OS_OSX
#    include "skappdelegate-c-interface.h"
#endif
#include "keysmodel.h"
#include "worker.h"

#include <QDebug>
#include <QGuiApplication>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    QLocalSocket *probe = new QLocalSocket();
    probe->connectToServer("SKApp");
    if (probe->waitForConnected(1000))
        return 0;
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("worker", new Worker());
    qmlRegisterUncreatableType<KeysModel>("SilverKey", 1, 0, "KeysModel", "C++ Only");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
