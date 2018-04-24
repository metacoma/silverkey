#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDebug>

#define KEYBOARD_SPEED 20

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

public:
    explicit MainWindow(QWidget *parent = 0);
    void EnterPressed();
    void EndOfWorkflow();
    void hideEvent(QHideEvent *e);
    void setWriteFd(int fd);

private:
    Q_OBJECT
    Ui::MainWindow *ui;
    QLineEdit *lineEdit;
    std::map<std::string, std::string> kvpairs;
    int wfd;


};

#endif // MAINWINDOW_H
