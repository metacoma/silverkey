#include "worker.h"

#include "focuscontroller.h"
#include "keysmodel.h"
#include "requester.h"
#include "uglobalhotkeys.h"

#include <Robot.h>
#include <chrono>
#ifdef Q_OS_MACOS
#    include <sys/wait.h>
#    include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#    include "focuscontroller_xcb.h"
#endif // Q_OS_LINUX

#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QMimeData>
#include <QSettings>

#ifdef Q_OS_MACOS
#    define PASTE_MODIFIER KeySystem
#    define PASTE_KEY "v"
#    elseif Q_OS_LINUX
#    define PASTE_MODIFIER KeyShift
#    define PASTE_KEY "{INSERT}"
#else
#    define PASTE_MODIFIER KeyControl
#    define PASTE_KEY "v"
#endif

inline QStringList getKeys(const QJsonObject &object)
{
    QStringList result;
    if (object.value("dir") == QJsonValue::Undefined) {
        result.append(object.value("key").toString());
    } else {
        const QJsonArray nodes = object.value("nodes").toArray();
        for (const auto &node : nodes)
            result += getKeys(node.toObject());
    }
    return result;
}

Worker::Worker(QObject *parent) : QObject(parent)
{
    m_focusController = new FocusController(this);
    setUpLocalServer();
    m_keysModel = new KeysModel({}, this);
    //createActions();
    //createTrayIcon();

    UGlobalHotkeys *hotkeyManager = new UGlobalHotkeys();
    //TODO: Make hotkey configurable
    hotkeyManager->registerHotkey("Ctrl+D");
    QObject::connect(hotkeyManager, &UGlobalHotkeys::activated, this, [this](size_t) {
        qDebug() << "I've got hotkey;";
        m_focusController->savePrevActive();
        //TODO: Show mainwin
    });

    m_httpClient = new Requester(this);
    connectToDb();

    setObjectName("skDialog");

    //    setAttribute(Qt::WA_TranslucentBackground);
    //    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    //    m_lineEdit = new FuzzyLineEdit(this);
    //    m_lineEdit->setObjectName("skInput");
    //    m_lineEdit->setFocusPolicy(Qt::StrongFocus);
    //    m_lineEdit->setFocus();
    //    m_lineEdit->setGeometry(0, 0, 500, 50);
    //    setRoundedCorners();
    //    m_lineEdit->setTextMargins(5, 0, 0, 0);
    //    m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(this, &Worker::dataLoaded, this, &Worker::handleDataLoad);
    //  connect(this, &Worker::gotReplyFromDb, this, &Worker::hide);
    connect(this, &Worker::gotDbUpdateEvent, this, &Worker::handleDbUpdate);
    connect(this, &Worker::gotDbUpdateError, this, &Worker::handleDbUpdateError);
    requestDbData();

    //    QMouseEvent pressEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, nullptr);
    //    qApp->sendEvent(m_lineEdit, &pressEvent);
    //    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, nullptr);
    //    qApp->sendEvent(m_lineEdit, &releaseEvent);

    //lockInput();
}

KeysModel *Worker::keysModel() const
{
    return m_keysModel;
}

void Worker::handleDataLoad(const QJsonObject &object)
{
    if (!object.isEmpty()) {
        m_wordList = getKeys(object.value("node").toObject());
        qDebug() << "Got obj" << m_wordList.join(" ");
        m_keysModel = new KeysModel(m_wordList, this);
        emit keysModelChanged();
    } else {
        //        int reply = QMessageBox::question(this, "Error", "Failed to load data from database", "Quit", "Open Settings");
        //        qDebug() << "Reply: " << reply;
        //        if (reply == 0) {
        //            m_lineEdit->setSelectedItem("");
        //            hide();
        //        } else {
        //            showSettings();
        //        }
    }

    //    if (!m_lineEdit->completer()->isDataSet()) {

    //    } else {
    //        unlockInput();
    //        waitForDbUdates();
    //    }
}

void Worker::updateDbIndex(int newIndex)
{
    m_dbIndex = newIndex;
}

void Worker::handleDbUpdate()
{
    requestDbData();
}

void Worker::handleDbUpdateError()
{
    waitForDbUdates();
}

void Worker::waitForDbUdates()
{
    qDebug() << "Start waiting for DB updates loop";
    Requester::handleFunction successHandler = [this](const QJsonObject &) {
        qDebug() << "Got data " << m_data;
        emit gotDbUpdateEvent();
    };

    Requester::handleFunction errorHandler = [this](const QJsonObject &) {
        qDebug() << "Error: connection dropped";
        emit gotDbUpdateError();
    };

    m_httpClient->sendRequest("v2/keys/?wait=true&recursive=true", successHandler, errorHandler);
}

void Worker::setUpLocalServer()
{
    m_server = new QLocalServer();
    QLocalServer::removeServer("SKApp");
    m_server->listen("SKApp");
    //connect(m_server, &QLocalServer::newConnection, this, &MainWindow::show);
}

void Worker::getValue(const QString &key)
{
    qDebug() << "Getting value from DB";
    Requester::handleFunction getData = [this](const QJsonObject &o) {
        m_data = o.value("node").toObject().value("value").toString();
        qDebug() << "Got data " << m_data;
        emit gotReplyFromDb();
    };

    Requester::handleFunction errData = [this](const QJsonObject &) {
        qDebug() << "Error retrieving key";
        emit gotReplyFromDb();
    };

    m_httpClient->sendRequest("v2/keys" + key, getData, errData);
}

void Worker::setValue(const QString &key, const QString &value)
{
    qDebug() << "Setting value to DB" << value;
    Requester::handleFunction getData = [this](const QJsonObject &o) {
        QString resp = o.value("node").toObject().value("value").toString();
        qDebug() << "Successfully written data" << resp;
        emit gotReplyFromDb();
    };

    Requester::handleFunction errData = [](const QJsonObject &) { qDebug() << "Error writing data"; };
    QString path;
    if (key[0] != '/') {
        path = "v2/keys/" + key;
    } else {
        path = "v2/keys" + key;
    }

    QByteArray encodedVal = QUrl::toPercentEncoding(value);
    m_httpClient->sendRequest(path, getData, errData, Requester::Type::PUT, "value=" + encodedVal);
}

void Worker::connectToDb()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    m_httpClient->initRequester(settings.value("server", "silverkey.app").toString(),
                                settings.value("port", 2379).toInt(), nullptr);
}

void Worker::savePreviouslyActiveWindow(const QString &bundleId)
{
#ifdef Q_OS_OSX
    focusController->setOldAppId(bundleID);
#else
    Q_UNUSED(bundleId);
#endif // Q_OS_OSX
}

void Worker::requestDbData()
{
    Requester::handleFunction successHandler = [this](const QJsonObject &object) { emit dataLoaded(object); };
    Requester::handleFunction errorHandler = [this](const QJsonObject &object) { emit dataLoaded(object); };

    m_httpClient->sendRequest("v2/keys/?recursive=true&sorted=true", successHandler, errorHandler);
}

void Worker::setData(const QString &data)
{
    m_data = data;
}

//void MainWindow::hideEvent(QHideEvent *event)
//{
//    if (!m_clipboardData->toPlainText().isEmpty()) {
//        qDebug() << "Hide action, value is " << m_data;

//        if (!m_data.isEmpty()) {
//            QClipboard *clipboard = QApplication::clipboard();
//            clipboard->setText(m_data);
//            m_focusController->switchFocus();

//#ifdef Q_OS_LINUX
//            Clipboard::SetText("Text");

//            clipboard->setText(m_data, QClipboard::Selection);
//            qDebug() << "Selection CB data" << clipboard->text(QClipboard::Selection);

//#endif
//            Keyboard keyboard;

//#ifdef Q_OS_OSX
//            std::this_thread::sleep_for(std::chrono::milliseconds(250));
//#endif
//            while (!keyboard.GetState(SK_PASTE_MODIFIER)) {
//                keyboard.Press(SK_PASTE_MODIFIER);
//                qDebug() << "Command key state " << keyboard.GetState(KeyShift);
//            }
//            keyboard.Click(SK_PASTE_KEY);
//            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//            keyboard.Release(SK_PASTE_MODIFIER);
//            m_lineEdit->setText("");
//#ifdef Q_OS_WIN
//            m_focusController->detachThread();
//#endif
//        }
//    }
//    hideTextEdit();
//    m_data = "";

//    event->accept();
//}

//void MainWindow::showEvent(QShowEvent *event)
//{
//    qDebug() << "Window show";
//    QFocusEvent *eventFocus = new QFocusEvent(QEvent::FocusIn);
//    qApp->postEvent(this, static_cast<QEvent *>(eventFocus), Qt::LowEventPriority);

//    QWidget::setFocusProxy(this);
//    m_lineEdit->setFocus();
//    updateWinPosition();
//#ifdef Q_OS_OSX
//    m_focusController->sendToFront();
//#endif
//    event->accept();
//}

//void MainWindow::escapePressed()
//{
//    m_clipboardData->setText("");
//    m_lineEdit->setText("");
//    m_lineEdit->setSelectedItem("");
//    m_focusController->switchFocus();
//    hide();
//}

//void MainWindow::enterPressed()
//{
//    qDebug() << "Enter pressed";

//    if (!m_clipboardData->toPlainText().isEmpty()) {
//        QString key = m_lineEdit->text();
//        setValue(key, m_clipboardData->toPlainText());
//    } else {
//        QString key = m_lineEdit->selectedItem();
//        getValue(key);
//    }
//}
