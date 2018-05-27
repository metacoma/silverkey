#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etcd.hpp"
#include "rapid_reply.hpp"
#include "fuzzycompleter.h"
#include "sksettings.h"
#include "hotkeys.h"

#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>
#include <QMenu>
#include <QSettings>
#include <QDesktopWidget>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent)
{
    setObjectName("skDialog");

    setStyleSheet("#skDialog {background:transparent;}");
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

    int x = settingsButton->x() +
            settingsButton->width() +
            widgetPadding +
            lineEdit->width() +
            widgetPadding;
    addDataButton->move(x, settingsButton->y());

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
    connect(lineEdit, &FuzzyLineEdit::hideApp, this, &MainWindow::hide);

    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);

    this->activateWindow();
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, (QEvent *)eventFocus, Qt::LowEventPriority);

    QPoint pos(lineEdit->width()-5, 5);
    QMouseEvent e(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &e);
    QMouseEvent f(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &f);

    QWidget::setFocusProxy(this);

    QPoint globalCursorPos = QCursor::pos();
    int mouseScreen = qApp->desktop()->screenNumber(globalCursorPos);
    qDebug() << "Screen " << mouseScreen;
    QRect ag = qApp->desktop()->screen(mouseScreen)->geometry();
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            ag
        )
    );

    lockInput();
    int w = settingsButton->width() +
            widgetPadding +
            lineEdit->width() +
            widgetPadding +
            addDataButton->width();
    resize(w,lineEdit->height());
    registerService();
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
    }

}

void MainWindow::getDbData()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QStringList wordList;

    try {
        etcd::Client<example::RapidReply> etcd_client(
                settings.value("server", "nseha.linkpc.net").toString().toStdString(),
                settings.value("port", 22379).toInt());
        example::RapidReply reply = etcd_client.GetAll("/");
        reply.GetAll(kvpairs);
        for (auto iter = kvpairs.begin(); iter != kvpairs.end(); ++iter) {
            wordList << QString::fromStdString(iter->first);
        }
        FuzzyCompleter *c = lineEdit->completer();
        c->setUp(wordList);
    } catch (etcd::ClientException e) {
        qDebug() << "Exception";
    }
}

void MainWindow::setResultPtr(QString *ptr)
{
    resultPtr = ptr;
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

void MainWindow::setData(std::string d) {
    data = d;
}

void MainWindow::hideEvent(QHideEvent *e) {
    std::string key = lineEdit->getSelectedItem().toStdString();
    if (data != "") {
        qDebug() << "Data from CLI: " << data.c_str();
        // TODO(dukov) Rework this to have only one connection to etcd
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        etcd::Client<example::RapidReply> etcd_client(
                    settings.value("server", "nseha.linkpc.net").toString().toStdString(),
                    settings.value("port", 22379).toInt());
        example::RapidReply reply = etcd_client.Set(key, data);
    } else {
        std::string val = "";
        if (kvpairs.count(key)) {
            val = kvpairs[key];
        }
        qDebug() << "Hide action, value is " << QString(val.c_str());
#ifdef Q_OS_MACOS
        write(wfd, val.c_str(), std::strlen(val.c_str()));
#endif
        if (resultPtr) {
            *resultPtr = QString::fromStdString(val);
        }
    }
    e->accept();
    qApp->closeAllWindows();
    qApp->exit();
}

void MainWindow::EnterPressed() {
    //this->~MainWindow();
    qDebug() << "EnterPressed";
    this->hide();
}

void MainWindow::SearchEvent() {
    FuzzyCompleter *c = lineEdit->completer();
    c->update(lineEdit->text());
    c->popup()->setCurrentIndex(c->popup()->model()->index(0,0));
}

void MainWindow::showSettings() {
    SKSettings s;
    int r = s.exec();
    qDebug() << "Settings result: " << r;
    if (r == QDialog::Accepted) {
        this->lockInput();
        lineEdit->completer()->cleanUp();
        this->getDbData();
        this->handleDataLoad();
    }
}

void MainWindow::setAngleCorners() {
    // TODO(dukov) get rid of this in favor of dynamic styles
    setStyleSheet("#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "border-bottom-right-radius: 0;"
                    "border-bottom-left-radius: 0;"
                    "font: 30pt Courier"
                  "}");
}

void MainWindow::setRoundedCorners() {
    // TODO(dukov) get rid of this in favor of dynamic styles
    setStyleSheet("#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "font: 30pt Courier"
                  "}");
}

