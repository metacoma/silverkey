#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class FocusController;
class KeysModel;
class DataManager;
class QQuickWindow;
class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KeysModel *keysModel READ keysModel NOTIFY keysModelChanged)

public:
    explicit Worker(QObject *parent = nullptr);
    KeysModel *keysModel() const;

    Q_INVOKABLE void registerHotKey(QQuickWindow *window);
    Q_INVOKABLE void insertData(const QString &rawData);
    Q_INVOKABLE void getValue(const QString &key);
    Q_INVOKABLE void escapePressed();
    Q_INVOKABLE void onRaise();

    Q_INVOKABLE void insertValue(const QString &value);

signals:
    void keysModelChanged();
    void raiseWindow();
    void hideWindow();
    void showErrorMessage(const QString &error);
    void valueLoaded(const QString &key, const QString &value);

private:
    void savePreviouslyActiveWindow(const QString &bundleId);

private:
    KeysModel *m_keysModel = nullptr;
    FocusController *m_focusController = nullptr;
    DataManager *m_dataManager = nullptr;
};

#endif // MAINWINDOW_H
