#ifndef MAINWINDOW_H

#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QLocalServer>
#include "fuzzycompleter.h"
#include "requester.h"
#include "focuscontroller.h"
#ifdef Q_OS_LINUX
#include "focuscontroller_xcb.h"
#endif // Q_OS_LINUX

#ifdef Q_OS_MACOS
#define SK_PASTE_MODIFIER KeySystem
#define SK_PASTE_KEY "v"
#elseif Q_OS_LINUX
#define SK_PASTE_MODIFIER KeyShift
#define SK_PASTE_KEY "{INSERT}"
#else
#define SK_PASTE_MODIFIER KeyControl
#define SK_PASTE_KEY "v"
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void EnterPressed();
    void EndOfWorkflow();
    void SearchEvent();
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *event);
    void setWriteFd(int fd);
    void setData(QString d);
    void getDbData();
    void showTextEdit();
    void hideTextEdit();
    static QStringList getKeys(const QJsonObject &o);
    void getVal(QString key);
    void setVal(QString key, QString val);
    void connectDB();
    void savePreviouslyActiveWindow(QString bundleID);

Q_SIGNALS:
    void dataLoaded();
    void gotReplyFromDB();
    void gotDbUpdateEvent();
    void gotDbUpdateError();

public slots:
    void escapePressed();
    void showSettings();
    void setAngleCorners();
    void setRoundedCorners();
    void handleDataLoad();
    void doHide();
    void quitApp();
    void updateDbIndex(int newIndex);
    void handleDbUpdate();
    void handleDbUpdateError();

private:
    void createTrayIcon();
    void createActions();
    void waitForDbUdates();
    void updateWinPosition();
    void setUpLocalServer();
    void lockInput();
    void unlockInput();

private:
    const int widgetPadding = 5;
    FuzzyLineEdit *lineEdit = nullptr;
    QPushButton *settingsButton = nullptr;
    QPushButton *addDataButton = nullptr;
    QTextEdit *clipboardData = nullptr;
    Requester *httpClient = nullptr;
    QStringList wordlist;
    int wfd = 0;
    QString data;
    FocusController *fc = nullptr;

    QAction *quitAction = nullptr;
    QAction *showAction = nullptr;
    QAction *hideAction = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayIconMenu = nullptr;
    int dbIndex = 0;
    QLocalServer *server = nullptr;
};

#endif // MAINWINDOW_H
