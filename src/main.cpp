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
#include <QClipboard>
ROBOT_NS_USE_ALL;


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

#ifdef SK_UI_FORK
void type_text(std::string dbVal) {

    qDebug() << "end of application workflow";

    if (dbVal.length()) {
        qDebug() << dbVal.c_str();

        Keyboard keyboard;

        sleep(1);
        keyboard.Click("$(v)");
    }
}
#endif

#ifdef SK_UI_FORK
void main_with_fork(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0) {
        close(fd[0]);
        show_window(argc, argv, fd[1], true);
        close(fd[1]);
    } else {
        close(fd[1]);
        wait(NULL);

        std::string dbVal;
        char ch;
        while (read(fd[0], &ch, 1) > 0)
        {
            if (ch != 0)
                dbVal.push_back(ch);

        }
        type_text(dbVal);
        close(fd[0]);
    }
}
#endif // SK_UI_FORK



int main(int argc, char *argv[])
{
#ifdef SK_UI_FORK
    main_with_fork(argc, argv);
#else
    show_window(argc, argv);
#endif // SK_UI_FORK
    return EXIT_SUCCESS;
}
