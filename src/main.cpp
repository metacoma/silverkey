#include "mainwindow.h"
#include "robothelper.h"
#include <iostream>
#include <QApplication>

#ifdef Q_OS_MACOS
# include <unistd.h>
# include <sys/wait.h>
#endif

#include <Robot.h>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
ROBOT_NS_USE_ALL;


void show_window(int argc, char *argv[], QString *res, int fd = 0, bool child = false) {
    QApplication a(argc, argv);
    MainWindow w;

    QFutureWatcher<void> watcher;
    QObject::connect(&watcher, SIGNAL(finished()), &w, SLOT(handleDataLoad()));
    QFuture<void> future = QtConcurrent::run(&w, &MainWindow::getDbData);
    watcher.setFuture(future);

    if (child) {
        w.setWriteFd(fd);
    }
    if (res) {
        w.setResultPtr(res);
    }
    if (argc == 2) {
        std::string d(argv[1]);
        w.setData(d);
    }
    w.setAttribute(Qt::WA_DeleteOnClose);

    w.show();

    w.raise();  // for MacOS
    w.activateWindow(); // for Windows

    qDebug() << a.exec();
}

void type_text(int dbValLen, char dbVal[256]) {

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

#ifdef SK_UI_FORK
void main_with_fork(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0) {
        close(fd[0]);
        show_window(argc, argv, NULL, fd[1], true);
        close(fd[1]);
    } else {
        close(fd[1]);
        wait(NULL);

        if (argc == 1) {
            char dbVal[256];
            int dbValLen;
            memset(&dbVal, 0, sizeof(dbVal));
            dbValLen = read(fd[0], &dbVal, sizeof(dbVal));
            type_text(dbValLen, dbVal);
        }
        close(fd[0]);
    }
}
#endif // SK_UI_FORK



int main(int argc, char *argv[])
{
#ifdef SK_UI_FORK
    main_with_fork(argc, argv);
#else
    QString res = "";
    show_window(argc, argv, &res);
    type_text(res.length(), (char *)res.toUtf8().constData());
#endif // SK_UI_FORK
    return EXIT_SUCCESS;
}
