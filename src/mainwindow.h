#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTextEdit>
#include "fuzzycompleter.h"
#include "requester.h"

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
    void setData(QString d);
    void getDbData();
    void showTextEdit();
    static QStringList getKeys(const QJsonObject &o);
    void getVal(QString key);
    void setVal(QString key, QString val);
    void connectDB();


Q_SIGNALS:
    void dataLoaded();
    void gotReplyFromDB();

public slots:
    void escapePressed();
    void showSettings();
    void setAngleCorners();
    void setRoundedCorners();
    void handleDataLoad();
    void doHide();


private:
    const int widgetPadding = 5;
    FuzzyLineEdit *lineEdit;
    QPushButton *settingsButton;
    QPushButton *addDataButton;
    QTextEdit *clipboardData;
    Requester *httpClient;
    void lockInput();
    void unlockInput();
    Q_OBJECT
    QStringList wordlist;
    int wfd;
    QString data = "";
};

#endif // MAINWINDOW_H
