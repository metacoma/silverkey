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

    Q_INVOKABLE void insertValue(const QString &key);
    Q_INVOKABLE void escapePressed();
    Q_INVOKABLE void sendResult();
    Q_INVOKABLE void onRaise();

signals:
    void dataLoaded(const QJsonObject &object);
    void dataLoadError();
    void gotReplyFromDb();
    void gotDbUpdateEvent();
    void gotDbUpdateError();
    void keysModelChanged();
    void raiseWindow();

private:
    void setData(const QString &m_data);
    void requestDbData();
    void showTextEdit();
    void hideTextEdit();
    void getValue(const QString &key);
    void setValue(const QString &key, const QString &value);
    void connectToDb();
    void savePreviouslyActiveWindow(const QString &bundleId);

    void handleDataLoad(const QJsonObject &object);
    void updateDbIndex(int newIndex);
    void handleDbUpdate();
    void handleDbUpdateError();

    void waitForDbUdates();
    void setUpLocalServer();

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
