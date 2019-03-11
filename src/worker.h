#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Requester;
class FocusController;
class QLocalServer;
class KeysModel;
class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KeysModel *keysModel READ keysModel NOTIFY keysModelChanged)

public:
    explicit Worker(QObject *parent = nullptr);
    KeysModel *keysModel() const;
signals:
    void dataLoaded(const QJsonObject &object);
    void gotReplyFromDb();
    void gotDbUpdateEvent();
    void gotDbUpdateError();
    void keysModelChanged();

private:
    void enterPressed();
    void endOfWorkflow();

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
    Requester *m_httpClient = nullptr;
    QStringList m_wordList;
    KeysModel *m_keysModel = nullptr;
    QString m_data;
    FocusController *m_focusController = nullptr;

    int m_dbIndex = 0;
    QLocalServer *m_server = nullptr;
};

#endif // MAINWINDOW_H
