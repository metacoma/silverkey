#include "mainwindow.h"
#ifdef Q_OS_OSX
# include "skappdelegate-c-interface.h"
#endif
#include <iostream>
#include <QApplication>
#include <Robot.h>
#include <QDebug>
#include <QLocalSocket>


void show_window(int argc, char *argv[], int fd = 0, bool child = false) {
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();
#ifdef Q_OS_OSX
    initSKAppDelegate(w);
#endif // Q_OS_OSX


    if (child) {
        w->setWriteFd(fd);
    }

    w->setAttribute(Qt::WA_DeleteOnClose);

    w->show();

    w->raise();  // for MacOS
    w->activateWindow(); // for Windows

    qDebug() << a.exec();
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    QLocalSocket *probe = new QLocalSocket();
    probe->connectToServer("SKApp");
    if (probe->waitForConnected(1000)) {
        return 0;
    }
#endif

    show_window(argc, argv);
    return EXIT_SUCCESS;
}
