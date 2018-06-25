#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fuzzycompleter.h"
#include "sksettings.h"
#include "hotkeys.h"
#include <Robot.h>
#include <chrono>
#include <thread>
#ifdef Q_OS_MACOS
# include <unistd.h>
# include <sys/wait.h>
#endif



#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>
#include <QMenu>
#include <QSettings>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QClipboard>
#include <QMimeData>
ROBOT_NS_USE_ALL;


MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent)
{
    fc = new FocusController();
    setUpLocalServer();
    createActions();
    createTrayIcon();


    httpClient = new Requester(this);
    connectDB();

    setObjectName("skDialog");

    //setStyleSheet("#skDialog {background:transparent;}");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    lineEdit = new FuzzyLineEdit(this);
    lineEdit->setObjectName("skInput");
    lineEdit->setFocusPolicy(Qt::StrongFocus);
    lineEdit->setFocus();
    lineEdit->setGeometry(0, 0, 500, 50);
    lineEdit->setStyleSheet(
                  "#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "font: 30pt Courier"
                  "}"
                );
    lineEdit->setTextMargins(5, 0, 0, 0);
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(this, &MainWindow::dataLoaded, this, &MainWindow::handleDataLoad);
    connect(this, &MainWindow::gotReplyFromDB, this, &MainWindow::doHide);
    connect(this, &MainWindow::gotDbUpdateEvent, this, &MainWindow::handleDbUpdate);
    connect(this, &MainWindow::gotDbUpdateError, this, &MainWindow::handleDbUpdateError);
    getDbData();

    settingsButton = new QPushButton("",this);
    settingsButton->setObjectName("settings");
    settingsButton->setStyleSheet("#settings {"
                                  "border-image:url(:/images/if_cog_35873.png);"
                                  "}"
                                  "#settings:pressed {"
                                  "border-image:url(:/images/if_cog_35873_pressed.png);"
                                  "}");
    settingsButton->resize(QImage(":/images/if_cog_35873.png").size());
    settingsButton->setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignTop|Qt::AlignLeft,
                    settingsButton->size(),
                    this->geometry()
                ));

    addDataButton = new QPushButton("", this);
    addDataButton->setObjectName("addData");
    addDataButton->setStyleSheet("#addData {"
                                 "border-image:url(:/images/if_edit_add_7710.png);"
                                 "}"
                                 "#addData:pressed {"
                                 "border-image:url(:/images/if_edit_add_7710_pressed.png);"
                                 "}");
    addDataButton->resize(QImage(":/images/if_edit_add_7710.png").size());


    lineEdit->move(settingsButton->x() + settingsButton->width() + widgetPadding,
                   settingsButton->y());

    int x = lineEdit->x() +
            lineEdit->width() +
            widgetPadding;
    addDataButton->move(x, settingsButton->y());


    clipboardData = new QTextEdit(this);
    clipboardData->setObjectName("clipboardData");
    clipboardData->setGeometry(x, settingsButton->y(), 500, 300);
    clipboardData->setStyleSheet(
                  "#clipboardData {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "font: 20pt Courier"
                  "}"
                );
    clipboardData->hide();

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
    lineEdit->setCompleter(completer);
    QAbstractItemView *abstractItemView = lineEdit->completer()->popup();


    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::EnterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::EnterPressed);
    connect(popup, &FuzzyPopup::popupShow, this, &MainWindow::setAngleCorners);
    connect(popup, &FuzzyPopup::popupHide, this, &MainWindow::setRoundedCorners);

    connect(lineEdit, &QLineEdit::textEdited, this, &MainWindow::SearchEvent);
    connect(lineEdit, &FuzzyLineEdit::hideApp, this, &MainWindow::escapePressed);

    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(addDataButton, &QPushButton::clicked, this, &MainWindow::showTextEdit);

    QPoint pos(lineEdit->width()-5, 5);
    QMouseEvent e(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &e);
    QMouseEvent f(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &f);

    lockInput();
    int w = settingsButton->width() +
            widgetPadding +
            lineEdit->width() +
            widgetPadding +
            addDataButton->width();
    resize(w,lineEdit->height());
}

QStringList MainWindow::getKeys(const QJsonObject &o) {
    QStringList res;
    if (o.value("dir") == QJsonValue::Undefined) {
        res.append(o.value("key").toString());
    } else {
        QJsonArray nodes = o.value("nodes").toArray();
        foreach(const QJsonValue &n, nodes) {
            res += getKeys(n.toObject());
        }
    }
    return res;
}

void MainWindow::handleDataLoad() {
    if (!lineEdit->completer()->isDataSet()) {
        //QMessageBox::StandardButton reply;
        int reply = QMessageBox::question(this,
                                      "Error", "Failed to load data from database",
                                      "Quit", "Open Settings");
        qDebug() << "Reply: " << reply;
        if (reply == 0) {
            lineEdit->setSelectedItem("");
            this->hide();
        } else {
            this->showSettings();
        }
    } else {
        this->unlockInput();
        waitForDbUdates();
    }

}

void MainWindow::doHide()
{
    qDebug() << "Hiding window";
    this->hide();
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon = QIcon(":/images/if_tray_active.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->show();
}

void MainWindow::createActions()
{
    quitAction = new QAction(tr("&Quit"), this);
    showAction = new QAction(tr("&Show"), this);
    connect(quitAction, &QAction::triggered, this, &MainWindow::quitApp);
    connect(showAction, &QAction::triggered, this, &MainWindow::show);
}

void MainWindow::quitApp()
{
    QLocalServer::removeServer("SKApp");
    qApp->closeAllWindows();
    qApp->exit();
}

void MainWindow::updateDbIndex(int newIndex)
{
    dbIndex = newIndex;
}

void MainWindow::handleDbUpdate()
{
    getDbData();
}

void MainWindow::handleDbUpdateError()
{
    waitForDbUdates();
}

void MainWindow::waitForDbUdates()
{
    qDebug() << "Start waiting for DB updates loop";
    Requester::handleFunc getData = [this](const QJsonObject &o) {
        qDebug() << "Got data " << this->data;
        emit this->gotDbUpdateEvent();
    };

    Requester::handleFunc errData = [this](const QJsonObject &o) {
        qDebug() << "Error: connection dropped";
        emit this->gotDbUpdateError();

    };

    httpClient->sendRequest("v2/keys/?wait=true&recursive=true",
                            getData,
                            errData);
}

void MainWindow::updateWinPosition()
{
    QPoint globalCursorPos = QCursor::pos();
    int mouseScreen = qApp->desktop()->screenNumber(globalCursorPos);
    qDebug() << "Screen " << mouseScreen;
    QRect ag = qApp->desktop()->screen(mouseScreen)->geometry();
    ag.setHeight(ag.height()/2);
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            ag
        )
                );
}

void MainWindow::setUpLocalServer()
{
    server = new QLocalServer();
    QLocalServer::removeServer("SKApp");
    server->listen("SKApp");
    connect(server, &QLocalServer::newConnection, this, &MainWindow::show);
}

void MainWindow::getVal(QString key) {
    qDebug() << "Getting value from DB";
    Requester::handleFunc getData = [this](const QJsonObject &o) {
        this->data = o.value("node").toObject().value("value").toString();
        qDebug() << "Got data " << this->data;
        emit this->gotReplyFromDB();
    };

    Requester::handleFunc errData = [this](const QJsonObject &o) {
        qDebug() << "Error retrieving key";
        emit this->gotReplyFromDB();
    };


    httpClient->sendRequest("v2/keys" + key, getData, errData);
}

void MainWindow::setVal(QString key, QString val) {
    qDebug() << "Setting value to DB" << val;
    Requester::handleFunc getData = [this](const QJsonObject &o) {
        QString resp = o.value("node").toObject().value("value").toString();
        qDebug() << "Successfully written data"<< resp;
        emit this->gotReplyFromDB();
    };

    Requester::handleFunc errData = [](const QJsonObject &o) {
        qDebug() << "Error writing data";
    };
    QString path;
    if (key[0] != '/') {
        path = "v2/keys/" + key;
    } else {
        path = "v2/keys" + key;
    }

    QByteArray encodedVal = QUrl::toPercentEncoding(val);
    httpClient->sendRequest(path,
                            getData,
                            errData,
                            Requester::Type::PUT,
                            "value=" + encodedVal);
}

void MainWindow::connectDB()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    httpClient->initRequester(settings.value("server", "hw2").toString(),
                              settings.value("port", 2379).toInt(),
                              nullptr);
}

void MainWindow::savePreviouslyActiveWindow(QString bundleID)
{
#ifdef Q_OS_OSX
    fc->setOldAppId(bundleID);
#endif // Q_OS_OSX
}


void MainWindow::getDbData()
{
    Requester::handleFunc getData = [this](const QJsonObject &o){
        this->wordlist = MainWindow::getKeys(o.value("node").toObject());
        qDebug() << "Got obj" << this->wordlist.join(" ");
        FuzzyCompleter *c = this->lineEdit->completer();
        c->setUp(this->wordlist);
        emit this->dataLoaded();
    };
    Requester::handleFunc errData = [this](const QJsonObject &o){
        qDebug() << "Got err obj";
        emit this->dataLoaded();
    };

    httpClient->sendRequest(
                "v2/keys/?recursive=true&sorted=true",
                getData,
                errData);

}

void MainWindow::lockInput()
{
    // TODO (dukov) Use gray style here
    lineEdit->setReadOnly(true);
    lineEdit->setText("Loading data...");
}

void MainWindow::unlockInput()
{
    // TODO (dukov) Restore default style
    lineEdit->setText("");
    lineEdit->setReadOnly(false);

}

void MainWindow::setWriteFd(int fd){
    wfd = fd;
}

void MainWindow::setData(QString d) {
    data = d;
}

void MainWindow::hideEvent(QHideEvent *e) {
    if (clipboardData->toPlainText() == "") {
        qDebug() << "Hide action, value is " << data;

        if (data != "") {
            QClipboard *cb = QApplication::clipboard();
            cb->setText(data);
            fc->switchFocus();

#ifdef Q_OS_LINUX
            Clipboard::SetText ("Text");

            cb->setText(data, QClipboard::Selection);
            qDebug() << "Selection CB data" << cb->text(QClipboard::Selection);

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
            lineEdit->setText("");
        }
    }
    hideTextEdit();
    data = "";

    e->accept();
}

void MainWindow::showEvent(QShowEvent *event)
{

    qDebug() << "Window show";
    this->activateWindow();
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, (QEvent *)eventFocus, Qt::LowEventPriority);

    QWidget::setFocusProxy(this);

    updateWinPosition();
    fc->savePrevActive();
#ifdef Q_OS_OSX
    fc->sendToFront();
#endif
    event->accept();
}

void MainWindow::escapePressed() {
    clipboardData->setText("");
    lineEdit->setText("");
    lineEdit->setSelectedItem("");
    lineEdit->completer()->popup()->hide();
    fc->switchFocus();
    this->hide();
}

void MainWindow::EnterPressed() {
    //this->~MainWindow();
    qDebug() << "EnterPressed";


    if (clipboardData->toPlainText() != "") {
        QString key = lineEdit->text();
        setVal(key, clipboardData->toPlainText());
    } else {
        QString key = lineEdit->getSelectedItem();
        getVal(key);
    }

}

void MainWindow::SearchEvent() {
    FuzzyCompleter *c = lineEdit->completer();
    c->update(lineEdit->text());
    c->popup()->setCurrentIndex(c->popup()->model()->index(0,0));
}

void MainWindow::showSettings() {
    SKSettings s;
    int r = s.exec();
    qDebug() << "Settings result: " << r << QDialog::Accepted;
    if (r == QDialog::Accepted) {
        this->lockInput();
        lineEdit->completer()->cleanUp();
        this->connectDB();
        this->getDbData();
    }
}

void MainWindow::hideTextEdit() {
    clipboardData->setText("");
    clipboardData->hide();
    addDataButton->show();
    int w = settingsButton->width() +
            widgetPadding +
            lineEdit->width() +
            widgetPadding +
            addDataButton->width();
    resize(w,lineEdit->height());

}

void MainWindow::showTextEdit() {
    addDataButton->hide();
    clipboardData->show();
    int w = settingsButton->width() +
            widgetPadding +
            lineEdit->width() +
            widgetPadding +
            clipboardData->width();
    resize(w,clipboardData->height());

    const QClipboard *cb = QApplication::clipboard();
    const QMimeData *md = cb->mimeData();
    if (md->hasText()) {
        clipboardData->setText(md->text());
    }

}

void MainWindow::setAngleCorners() {
    // TODO(dukov) get rid of this in favor of dynamic styles
    lineEdit->setStyleSheet("#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "border-bottom-right-radius: 0;"
                    "border-bottom-left-radius: 0;"
                    "font: 30pt Courier"
                  "}");
}

void MainWindow::setRoundedCorners() {
    // TODO(dukov) get rid of this in favor of dynamic styles
    lineEdit->setStyleSheet("#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "font: 30pt Courier"
                  "}");
}

