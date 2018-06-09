#include "mainwindow.h"
#include "robothelper.h"
#include <iostream>
#include <QApplication>
#include <Robot.h>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QClipboard>
#include <QLocalSocket>

void show_window(int argc, char *argv[], int fd = 0, bool child = false) {
    QApplication a(argc, argv);
    MainWindow w;

    if (child) {
        w.setWriteFd(fd);
    }

    w.setAttribute(Qt::WA_DeleteOnClose);

    w.show();

    w.raise();  // for MacOS
    w.activateWindow(); // for Windows

    qDebug() << a.exec();
}

int main(int argc, char *argv[])
{
    QLocalSocket *probe = new QLocalSocket();
    probe->connectToServer("SKApp");
    if (probe->waitForConnected(1000)) {
        return 0;
    }
    show_window(argc, argv);
    return EXIT_SUCCESS;
}
