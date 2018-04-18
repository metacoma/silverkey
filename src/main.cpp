#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include <unistd.h>
#include <sys/wait.h>
#include <Robot.h>
ROBOT_NS_USE_ALL;



int main(int argc, char *argv[])
{
    int fd[2];
    QString dbVal;
    int status;
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        QApplication a(argc, argv);
        MainWindow w;
        w.setWriteFd(fd[1]);
        w.show();


        w.raise();  // for MacOS
        w.activateWindow(); // for Windows
      
        a.exec();
        close(fd[1]);
    } else {
        close(fd[1]);
        while (true) {
            read(fd[0], &dbVal, sizeof(QString));
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == -1) {
                break;
            } else if (result != 0) {
                break;
            }

        }

        close(fd[0]);
        qDebug() << "end of application workflow";
        qDebug() << dbVal.toUtf8().constData();
        Keyboard keyboard;
        keyboard.Click ("hello world sample");
    }
    return EXIT_SUCCESS;
}
