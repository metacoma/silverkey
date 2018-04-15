#include "mainwindow.h"
#include <iostream>
#include <QApplication>

#include <Robot.h>
ROBOT_NS_USE_ALL;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;


    w.show();

    a.exec();

    qDebug() << "end of application workflow";
    return EXIT_SUCCESS;
}
