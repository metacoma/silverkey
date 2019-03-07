#include "mainwindow.h"
#ifdef Q_OS_OSX
#    include "skappdelegate-c-interface.h"
#endif
#include <QApplication>
#include <QDebug>
#include <QLocalSocket>

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    QLocalSocket *probe = new QLocalSocket();
    probe->connectToServer("SKApp");
    if (probe->waitForConnected(1000))
        return 0;
#endif

    QApplication a(argc, argv);
    MainWindow *window = new MainWindow();
#ifdef Q_OS_OSX
    initSKAppDelegate(w);
#endif // Q_OS_OSX

    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
    window->raise(); // for MacOS
    window->activateWindow(); // for Windows
    window->hide();
    return a.exec();
}
