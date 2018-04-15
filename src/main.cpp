#include "mainwindow.h"
#include <iostream>
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;


    w.show();

    w.raise();  // for MacOS
    w.activateWindow(); // for Windows

    a.exec();

    qDebug() << "end of application workflow";
    return EXIT_SUCCESS;
}
