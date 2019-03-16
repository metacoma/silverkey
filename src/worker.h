#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class FocusController;
class KeysModel;
class DataManager;
class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KeysModel *keysModel READ keysModel NOTIFY keysModelChanged)

public:
    explicit Worker(QObject *parent = nullptr);
    KeysModel *keysModel() const;

    Q_INVOKABLE void insertValue(const QString &key);
    Q_INVOKABLE void escapePressed();
    Q_INVOKABLE void onRaise();

    void setValue(const QString &value);

signals:
    void keysModelChanged();
    void raiseWindow();
    void hideWindow();
    void showErrorMessage(const QString &error);

private:
    void savePreviouslyActiveWindow(const QString &bundleId);

private:
    KeysModel *m_keysModel = nullptr;
    FocusController *m_focusController = nullptr;
    DataManager *m_dataManager = nullptr;
};

#endif // MAINWINDOW_H
