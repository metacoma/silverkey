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
        w.setWriteFd(fd[1]);
        w.show();


        w.raise();  // for MacOS
        w.activateWindow(); // for Windows
      
        a.exec();
        close(fd[1]);
    } else {
        close(fd[1]);
        while (true) {

            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == -1) {
                break;
            } else if (result != 0) {
                break;
            }

        }
        dbValLen = read(fd[0], &dbVal, sizeof(dbVal));
        close(fd[0]);
        qDebug() << "end of application workflow";
        if (dbValLen) {
            qDebug() << dbVal;
            Keyboard keyboard;
            keyboard.Click (dbVal);
        }


    }
    return EXIT_SUCCESS;
}
