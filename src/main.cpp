#include "mainwindow.h"
#include "robothelper.h"
#include <iostream>
#include <QApplication>
#include <unistd.h>
#include <sys/wait.h>
#include <Robot.h>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
ROBOT_NS_USE_ALL;



int main(int argc, char *argv[])
{
    int fd[2];
    char dbVal[256];
    int dbValLen;
    int status;
    pipe(fd);
    pid_t pid = fork();

    memset(&dbVal, 0, sizeof(dbVal));
    if (pid == 0) {
        close(fd[0]);
        QApplication a(argc, argv);
        MainWindow w;

        QFutureWatcher<void> watcher;
        QObject::connect(&watcher, SIGNAL(finished()), &w, SLOT(handleDataLoad()));
        QFuture<void> future = QtConcurrent::run(&w, &MainWindow::getDbData);
        watcher.setFuture(future);

        w.setWriteFd(fd[1]);
        if (argc == 2) {
            std::string d(argv[1]);
            w.setData(d);
        }
        w.setAttribute(Qt::WA_DeleteOnClose);

        w.show();

        w.raise();  // for MacOS
        w.activateWindow(); // for Windows
      
        qDebug() << a.exec();
        close(fd[1]);
    } else {
        close(fd[1]);
        wait(NULL);

        if (argc == 1) {
            dbValLen = read(fd[0], &dbVal, sizeof(dbVal));
            qDebug() << "end of application workflow";

            if (dbValLen) {
                qDebug() << dbVal;

                std::string value(dbVal);

                RobotHelper helper;
                auto converted = helper.convert(value);
                const char* c_converted = converted.c_str();

                Keyboard keyboard;

                keyboard.AutoDelay.Min = 2;
                keyboard.AutoDelay.Max = 3;

                keyboard.Click (c_converted);
                //This one easily misses some key presses. Need to get rid of it asap.
                //Does not support multiline properly eather.
            }

        }
        close(fd[0]);

    }
    return EXIT_SUCCESS;
}
