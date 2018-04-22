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
        w.setAttribute(Qt::WA_DeleteOnClose);

        w.show();

        w.raise();  // for MacOS
        w.activateWindow(); // for Windows
      
        qDebug() << a.exec();
        close(fd[1]);
    } else {
        close(fd[1]);
        wait(NULL);
        dbValLen = read(fd[0], &dbVal, sizeof(dbVal));
        close(fd[0]);
        qDebug() << "end of application workflow";

        if (dbValLen) {
            qDebug() << dbVal;
            Keyboard keyboard;
          
            keyboard.AutoDelay.Min = std::max(KEYBOARD_SPEED - 5, 1);
            keyboard.AutoDelay.Max = std::max(KEYBOARD_SPEED + 5, 1);
            keyboard.Click (dbVal);
        }

    }
    return EXIT_SUCCESS;
}
