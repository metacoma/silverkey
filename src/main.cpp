#ifdef Q_OS_OSX
#    include "skappdelegate-c-interface.h"
#endif
#include "keysmodel.h"
#include "worker.h"

#include <QGuiApplication>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    // Single instance app support
    QLocalSocket *probe = new QLocalSocket();
    probe->connectToServer("SKApp");
    if (probe->waitForConnected(1000))
        return 0;

    delete probe;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("worker", new Worker(&engine));
    qmlRegisterUncreatableType<KeysModel>("SilverKey", 1, 0, "KeysModel", "C++ Only");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
