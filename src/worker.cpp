#include "worker.h"

#include "datamanager.h"
#include "focuscontroller.h"
#include "keysmodel.h"
#include "uglobalhotkeys.h"

#include <Robot.h>
#include <chrono>
#include <thread>
#ifdef Q_OS_MACOS
#    include <sys/wait.h>
#    include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#    include "focuscontroller_xcb.h"
#endif // Q_OS_LINUX

#include <QClipboard>
#include <QDebug>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QMimeData>
#include <QSettings>
#include <QQuickWindow>

#ifdef Q_OS_MACOS
#    define PASTE_MODIFIER KeySystem
#    define PASTE_KEY "v"
#    elseif Q_OS_LINUX
#    define PASTE_MODIFIER KeyShift
#    define PASTE_KEY "{INSERT}"
#else
#    define PASTE_MODIFIER Robot::KeyControl
#    define PASTE_KEY "v"
#endif

Worker::Worker(QObject *parent) : QObject(parent)
{
    auto server = new QLocalServer();
    QLocalServer::removeServer("SKApp");
    server->listen("SKApp");

    m_focusController = new FocusController(this);
    m_keysModel = new KeysModel({}, this);
    m_dataManager = new DataManager(this);

    connect(m_dataManager, &DataManager::wordListUpdated, this, [this](const QStringList &wordList) {
        if (m_keysModel)
            delete m_keysModel;
        m_keysModel = new KeysModel(wordList, this);
        emit keysModelChanged();
    });

    connect(m_dataManager, &DataManager::valueSaved, this, [this](const QString &key, const QString &) {
        if (!m_keysModel)
            return;
        m_keysModel->append(key);
    });

    connect(m_dataManager, &DataManager::valueLoaded, this, &Worker::valueLoaded);
    connect(m_dataManager, &DataManager::wordListLoadError, this,
            [this]() { emit showErrorMessage(tr("Can't load word list from server")); });
    connect(m_dataManager, &DataManager::updateRequestError,
            [this]() { emit showErrorMessage(tr("Can't connect to server")); });
    connect(m_dataManager, &DataManager::writeToServerError, this,
            [this]() { emit showErrorMessage(tr("Can't write value to server")); });
    connect(m_dataManager, &DataManager::loadFromServerError,
            [this]() { emit showErrorMessage(tr("Can't load value from server")); });

    //TODO: (for Mac developer) check in qml: m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
}

KeysModel *Worker::keysModel() const
{
    return m_keysModel;
}

void Worker::registerHotKey(QQuickWindow *window)
{
    if (!window){
        qDebug() << "Window is null!";
        return;
    }

    WId winId = window->winId();
    qDebug() << "winId=" << winId << window->objectName();

    auto *hotkeyManager = new UGlobalHotkeys(this, winId);
    //TODO: Make hotkey configurable
    hotkeyManager->registerHotkey("Ctrl+D");
    QObject::connect(hotkeyManager, &UGlobalHotkeys::hideWindow, this, &Worker::hideWindow);
    QObject::connect(hotkeyManager, &UGlobalHotkeys::activated, this, [this](size_t) {
        qDebug() << "I've got hotkey;";
        m_focusController->savePrevActive();
        emit raiseWindow();
    });
}

void Worker::insertData(const QString &rawData)
{
    m_dataManager->insertData(rawData);
}

void Worker::getValue(const QString &key)
{
    qDebug() << "Insert value for key:" << key;
    m_dataManager->requestValue(key);
}

void Worker::savePreviouslyActiveWindow(const QString &bundleId)
{
#ifdef Q_OS_OSX
    focusController->setOldAppId(bundleID);
#else
    Q_UNUSED(bundleId);
#endif // Q_OS_OSX
}

void Worker::insertValue(const QString &value)
{
    qDebug() << "Hide action, value is " << value;
    emit hideWindow();
    if (!value.isEmpty()) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(value);
        m_focusController->switchFocus();

#ifdef Q_OS_LINUX
        Robot::Clipboard::SetText("Text");

        clipboard->setText(value, QClipboard::Selection);
        qDebug() << "Selection CB data" << clipboard->text(QClipboard::Selection);

#endif
        Robot::Keyboard keyboard;

#ifdef Q_OS_OSX
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
#endif
        while (!keyboard.GetState(PASTE_MODIFIER)) {
            keyboard.Press(PASTE_MODIFIER);
            qDebug() << "Command key state " << keyboard.GetState(PASTE_MODIFIER);
        }
        keyboard.Click(PASTE_KEY);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        keyboard.Release(PASTE_MODIFIER);
#ifdef Q_OS_WIN
        m_focusController->detachThread();
#endif
    }
}

void Worker::onRaise()
{
    qDebug() << "Reise, Reise";
    QFocusEvent *eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, static_cast<QEvent *>(eventFocus), Qt::LowEventPriority);

#ifdef Q_OS_OSX
    m_focusController->sendToFront();
#endif
}

void Worker::escapePressed()
{
    m_focusController->switchFocus();
}
