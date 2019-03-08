#include "mainwindow.h"

#include "fuzzycompleter.h"
#include "sksettings.h"
#include "uglobalhotkeys.h"

#include <Robot.h>
#include <chrono>
#include <thread>
#ifdef Q_OS_MACOS
#    include <sys/wait.h>
#    include <unistd.h>
#endif

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSettings>

ROBOT_NS_USE_ALL;

#ifdef Q_OS_MACOS
#    define SK_PASTE_MODIFIER KeySystem
#    define SK_PASTE_KEY "v"
#    elseif Q_OS_LINUX
#    define SK_PASTE_MODIFIER KeyShift
#    define SK_PASTE_KEY "{INSERT}"
#else
#    define SK_PASTE_MODIFIER KeyControl
#    define SK_PASTE_KEY "v"
#endif

constexpr int WIDGET_PADDING = 5;

QStringList getKeys(const QJsonObject &object)
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

MainWindow::MainWindow(QWidget *parent) : QDialog(parent)
{
    m_focusController = new FocusController();
    setUpLocalServer();
    createActions();
    createTrayIcon();

    UGlobalHotkeys *hotkeyManager = new UGlobalHotkeys();
    //TODO: Make hotkey configurable
    hotkeyManager->registerHotkey("Ctrl+D");
    QObject::connect(hotkeyManager, &UGlobalHotkeys::activated, this, [this](size_t) {
        qDebug() << "I've got hotkey;";
        m_focusController->savePrevActive();
        raise();
        activateWindow();
        show();
    });

    m_httpClient = new Requester(this);
    connectToDb();

    setObjectName("skDialog");

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    m_lineEdit = new FuzzyLineEdit(this);
    m_lineEdit->setObjectName("skInput");
    m_lineEdit->setFocusPolicy(Qt::StrongFocus);
    m_lineEdit->setFocus();
    m_lineEdit->setGeometry(0, 0, 500, 50);
    setRoundedCorners();
    m_lineEdit->setTextMargins(5, 0, 0, 0);
    m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(this, &MainWindow::dataLoaded, this, &MainWindow::handleDataLoad);
    connect(this, &MainWindow::gotReplyFromDb, this, &MainWindow::hide);
    connect(this, &MainWindow::gotDbUpdateEvent, this, &MainWindow::handleDbUpdate);
    connect(this, &MainWindow::gotDbUpdateError, this, &MainWindow::handleDbUpdateError);
    requestDbData();

    m_settingsButton = new QPushButton("", this);
    m_settingsButton->setObjectName("settings");
    m_settingsButton->setStyleSheet("#settings {"
                                    "border-image:url(:/images/gear.png);"
                                    "}"
                                    "#settings:pressed {"
                                    "border-image:url(:/images/gear-pressed.png);"
                                    "}");
    m_settingsButton->resize(QImage(":/images/gear.png").size());
    m_settingsButton->setGeometry(
        QStyle::alignedRect(Qt::LeftToRight, Qt::AlignTop | Qt::AlignLeft, m_settingsButton->size(), geometry()));

    m_addDataButton = new QPushButton("", this);
    m_addDataButton->setObjectName("addData");
    m_addDataButton->setStyleSheet("#addData {"
                                   "border-image:url(:/images/add.png);"
                                   "}"
                                   "#addData:pressed {"
                                   "border-image:url(:/images/add-pressed.png);"
                                   "}");
    m_addDataButton->resize(QImage(":/images/add.png").size());

    m_lineEdit->move(m_settingsButton->x() + m_settingsButton->width() + WIDGET_PADDING, m_settingsButton->y());

    int x = m_lineEdit->x() + m_lineEdit->width() + WIDGET_PADDING;
    m_addDataButton->move(x, m_settingsButton->y());

    m_clipboardData = new QTextEdit(this);
    m_clipboardData->setObjectName("clipboardData");
    m_clipboardData->setGeometry(x, m_settingsButton->y(), 500, 300);
    m_clipboardData->setStyleSheet("#clipboardData {"
                                   "background-color: #f6f6f6;"
                                   "border-radius: 10px;"
                                   "font: 20pt Courier"
                                   "}");
    m_clipboardData->hide();

    FuzzyCompleter *completer = new FuzzyCompleter(this);
    FuzzyPopup *popup = new FuzzyPopup();
    popup->setObjectName("skPopup");
    popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setSelectionBehavior(QAbstractItemView::SelectRows);
    popup->setSelectionMode(QAbstractItemView::SingleSelection);
    completer->setPopup(popup);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->popup()->setStyleSheet("#skPopup {"
                                      "background-color: #f6f6f6;"
                                      "font: 20pt Courier"
                                      "}");
    m_lineEdit->setCompleter(completer);
    QAbstractItemView *abstractItemView = m_lineEdit->completer()->popup();

    connect(m_lineEdit, &QLineEdit::returnPressed, this, &MainWindow::enterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::enterPressed);
    connect(popup, &FuzzyPopup::popupShow, this, &MainWindow::setAngleCorners);
    connect(popup, &FuzzyPopup::popupHide, this, &MainWindow::setRoundedCorners);

    connect(m_lineEdit, &QLineEdit::textEdited, this, &MainWindow::searchEvent);
    connect(m_lineEdit, &FuzzyLineEdit::hideApplication, this, &MainWindow::escapePressed);

    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(m_addDataButton, &QPushButton::clicked, this, &MainWindow::showTextEdit);

    QPoint pos(m_lineEdit->width() - 5, 5);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, nullptr);
    qApp->sendEvent(m_lineEdit, &pressEvent);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, nullptr);
    qApp->sendEvent(m_lineEdit, &releaseEvent);

    lockInput();
    int width = m_settingsButton->width() + WIDGET_PADDING + m_lineEdit->width() + WIDGET_PADDING
                + m_addDataButton->width();
    resize(width, m_lineEdit->height());
}

void MainWindow::handleDataLoad()
{
    if (!m_lineEdit->completer()->isDataSet()) {
        int reply = QMessageBox::question(this, "Error", "Failed to load data from database", "Quit", "Open Settings");
        qDebug() << "Reply: " << reply;
        if (reply == 0) {
            m_lineEdit->setSelectedItem("");
            hide();
        } else {
            showSettings();
        }
    } else {
        unlockInput();
        waitForDbUdates();
    }
}

void MainWindow::createTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_showAction);
    m_trayIconMenu->addAction(m_hideAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);

    QIcon icon = QIcon(":/images/tray.png");
    m_trayIcon->setIcon(icon);
    setWindowIcon(icon);
    m_trayIcon->show();
}

void MainWindow::createActions()
{
    m_quitAction = new QAction(tr("&Quit"), this);
    m_showAction = new QAction(tr("&Show"), this);
    m_hideAction = new QAction(tr("&Hide"), this);
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::quitApplication);
    connect(m_showAction, &QAction::triggered, this, &MainWindow::show);
    connect(m_hideAction, &QAction::triggered, this, &MainWindow::hide);
}

void MainWindow::quitApplication()
{
    QLocalServer::removeServer("SKApp");
    qApp->closeAllWindows();
    qApp->exit();
}

void MainWindow::updateDbIndex(int newIndex)
{
    m_dbIndex = newIndex;
}

void MainWindow::handleDbUpdate()
{
    requestDbData();
}

void MainWindow::handleDbUpdateError()
{
    waitForDbUdates();
}

void MainWindow::waitForDbUdates()
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

void MainWindow::updateWinPosition()
{
    QPoint globalCursorPos = QCursor::pos();
    int mouseScreen = qApp->desktop()->screenNumber(globalCursorPos);
    qDebug() << "Screen " << mouseScreen;
    QRect newRect = qApp->desktop()->screen(mouseScreen)->geometry();
    newRect.setHeight(newRect.height() / 2);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), newRect));
}

void MainWindow::setUpLocalServer()
{
    m_server = new QLocalServer();
    QLocalServer::removeServer("SKApp");
    m_server->listen("SKApp");
    connect(m_server, &QLocalServer::newConnection, this, &MainWindow::show);
}

void MainWindow::getValue(const QString &key)
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

void MainWindow::setValue(const QString &key, const QString &value)
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

void MainWindow::connectToDb()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    m_httpClient->initRequester(settings.value("server", "silverkey.app").toString(),
                                settings.value("port", 2379).toInt(), nullptr);
}

void MainWindow::savePreviouslyActiveWindow(const QString &bundleId)
{
#ifdef Q_OS_OSX
    fc->setOldAppId(bundleID);
#else
    Q_UNUSED(bundleId);
#endif // Q_OS_OSX
}

void MainWindow::requestDbData()
{
    Requester::handleFunction successHandler = [this](const QJsonObject &object) {
        m_wordList = getKeys(object.value("node").toObject());
        qDebug() << "Got obj" << m_wordList.join(" ");
        FuzzyCompleter *completer = m_lineEdit->completer();
        completer->setUp(m_wordList);
        emit dataLoaded();
    };
    Requester::handleFunction errorHandler = [this](const QJsonObject &) {
        qDebug() << "Got err obj";
        emit dataLoaded();
    };

    m_httpClient->sendRequest("v2/keys/?recursive=true&sorted=true", successHandler, errorHandler);
}

void MainWindow::lockInput()
{
    // TODO (dukov) Use gray style here
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setText("Loading data...");
}

void MainWindow::unlockInput()
{
    // TODO (dukov) Restore default style
    m_lineEdit->setText("");
    m_lineEdit->setReadOnly(false);
}

void MainWindow::setData(const QString &data)
{
    m_data = data;
}

void MainWindow::hideEvent(QHideEvent *event)
{
    if (!m_clipboardData->toPlainText().isEmpty()) {
        qDebug() << "Hide action, value is " << m_data;

        if (!m_data.isEmpty()) {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(m_data);
            m_focusController->switchFocus();

#ifdef Q_OS_LINUX
            Clipboard::SetText("Text");

            clipboard->setText(m_data, QClipboard::Selection);
            qDebug() << "Selection CB data" << clipboard->text(QClipboard::Selection);

#endif
            Keyboard keyboard;

#ifdef Q_OS_OSX
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
#endif
            while (!keyboard.GetState(SK_PASTE_MODIFIER)) {
                keyboard.Press(SK_PASTE_MODIFIER);
                qDebug() << "Command key state " << keyboard.GetState(KeyShift);
            }
            keyboard.Click(SK_PASTE_KEY);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            keyboard.Release(SK_PASTE_MODIFIER);
            m_lineEdit->setText("");
#ifdef Q_OS_WIN
            m_focusController->detachThread();
#endif
        }
    }
    hideTextEdit();
    m_data = "";

    event->accept();
}

void MainWindow::showEvent(QShowEvent *event)
{
    qDebug() << "Window show";
    QFocusEvent *eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, static_cast<QEvent *>(eventFocus), Qt::LowEventPriority);

    QWidget::setFocusProxy(this);
    m_lineEdit->setFocus();
    updateWinPosition();
#ifdef Q_OS_OSX
    m_focusController->sendToFront();
#endif
    event->accept();
}

void MainWindow::escapePressed()
{
    m_clipboardData->setText("");
    m_lineEdit->setText("");
    m_lineEdit->setSelectedItem("");
    m_lineEdit->completer()->popup()->hide();
    m_focusController->switchFocus();
    hide();
}

void MainWindow::enterPressed()
{
    qDebug() << "Enter pressed";

    if (!m_clipboardData->toPlainText().isEmpty()) {
        QString key = m_lineEdit->text();
        setValue(key, m_clipboardData->toPlainText());
    } else {
        QString key = m_lineEdit->selectedItem();
        getValue(key);
    }
}

void MainWindow::searchEvent()
{
    FuzzyCompleter *completer = m_lineEdit->completer();
    completer->update(m_lineEdit->text());
    completer->popup()->setCurrentIndex(completer->popup()->model()->index(0, 0));
}

void MainWindow::focusOutEvent(QFocusEvent *)
{
    hide();
}

void MainWindow::showSettings()
{
    SKSettings settings;
    int result = settings.exec();
    qDebug() << "Settings result: " << result << QDialog::Accepted;
    if (result == QDialog::Accepted) {
        lockInput();
        m_lineEdit->completer()->cleanUp();
        connectToDb();
        requestDbData();
    }
}

void MainWindow::hideTextEdit()
{
    m_clipboardData->setText("");
    m_clipboardData->hide();
    m_addDataButton->show();
    int width = m_settingsButton->width() + WIDGET_PADDING + m_lineEdit->width() + WIDGET_PADDING
                + m_addDataButton->width();
    resize(width, m_lineEdit->height());
}

void MainWindow::showTextEdit()
{
    m_addDataButton->hide();
    m_clipboardData->show();
    int width = m_settingsButton->width() + WIDGET_PADDING + m_lineEdit->width() + WIDGET_PADDING
                + m_clipboardData->width();
    resize(width, m_clipboardData->height());

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasText()) {
        m_clipboardData->setText(mimeData->text());
    }
}

void MainWindow::setAngleCorners()
{
    // TODO(dukov) get rid of this in favor of dynamic styles
    m_lineEdit->setStyleSheet("#skInput {"
                              "background-color: #f6f6f6;"
                              "border-radius: 10px;"
                              "border-bottom-right-radius: 0;"
                              "border-bottom-left-radius: 0;"
                              "font: 30pt Courier"
                              "}");
}

void MainWindow::setRoundedCorners()
{
    // TODO(dukov) get rid of this in favor of dynamic styles
    m_lineEdit->setStyleSheet("#skInput {"
                              "background-color: #f6f6f6;"
                              "border-radius: 10px;"
                              "font: 30pt Courier"
                              "}");
}
