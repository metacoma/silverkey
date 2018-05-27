#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include "fuzzycompleter.h"

#define KEYBOARD_SPEED 20

#ifdef Q_OS_MACOS
#define SK_UI_FORK
#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{

public:
    explicit MainWindow(QWidget *parent = 0);
    void EnterPressed();
    void EndOfWorkflow();
    void SearchEvent();
    void hideEvent(QHideEvent *e);
    void setWriteFd(int fd);
    void setData(std::string d);
    void getDbData();
    void setResultPtr(QString *ptr);
public slots:
    void showSettings();
    void setAngleCorners();
    void setRoundedCorners();
    void handleDataLoad();

private:
    FuzzyLineEdit *lineEdit;
    void lockInput();
    void unlockInput();
    Q_OBJECT
    std::map<std::string, std::string> kvpairs;
    int wfd;
    std::string data = "";
    QString *resultPtr;

};

#endif // MAINWINDOW_H
