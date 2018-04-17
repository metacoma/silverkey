#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include <unistd.h>
#include <sys/wait.h>
#include <Robot.h>
ROBOT_NS_USE_ALL;


int main(int argc, char *argv[])
{
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        QApplication a(argc, argv);
        MainWindow w;

        w.show();


        w.raise();  // for MacOS
        w.activateWindow(); // for Windows
      
        a.exec();
    } else {
        while (wait(&status) != pid)
            ;

        qDebug() << "end of application workflow";
        Keyboard keyboard;
        keyboard.Click ("hello world sample");
    }
    return EXIT_SUCCESS;
}
