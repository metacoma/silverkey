#ifndef MAINWINDOW_H

#define MAINWINDOW_H

#include "focuscontroller.h"
#include "fuzzycompleter.h"
#include "requester.h"

#include <QAction>
#include <QDialog>
#include <QLocalServer>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTextEdit>
#ifdef Q_OS_LINUX
#    include "focuscontroller_xcb.h"
#endif // Q_OS_LINUX

class MainWindow : public QDialog
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

Q_SIGNALS:
    void dataLoaded();
    void gotReplyFromDb();
    void gotDbUpdateEvent();
    void gotDbUpdateError();

private:
    void enterPressed();
    void endOfWorkflow();
    void searchEvent();
    void focusOutEvent(QFocusEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void setData(const QString &m_data);
    void requestDbData();
    void showTextEdit();
    void hideTextEdit();
    void getValue(const QString &key);
    void setValue(const QString &key, const QString &value);
    void connectToDb();
    void savePreviouslyActiveWindow(const QString &bundleId);

    void escapePressed();
    void showSettings();
    void setAngleCorners();
    void setRoundedCorners();
    void handleDataLoad();
    void quitApplication();
    void updateDbIndex(int newIndex);
    void handleDbUpdate();
    void handleDbUpdateError();

    void createTrayIcon();
    void createActions();
    void waitForDbUdates();
    void updateWinPosition();
    void setUpLocalServer();
    void lockInput();
    void unlockInput();

private:
    FuzzyLineEdit *m_lineEdit = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_addDataButton = nullptr;
    QTextEdit *m_clipboardData = nullptr;
    Requester *m_httpClient = nullptr;
    QStringList m_wordList;
    QString m_data;
    FocusController *m_focusController = nullptr;

    QAction *m_quitAction = nullptr;
    QAction *m_showAction = nullptr;
    QAction *m_hideAction = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayIconMenu = nullptr;
    int m_dbIndex = 0;
    QLocalServer *m_server = nullptr;
};

#endif // MAINWINDOW_H
