#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etcd.hpp"
#include "rapid_reply.hpp"
#include "fuzzycompleter.h"
#include "sksettings.h"

#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>
#include <QMenu>
#include <QSettings>
#include <QDesktopWidget>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    FuzzyLineEdit(parent),

    settingsAcc(new QAction(tr("&Settings"), this))
{
    setObjectName("skInput");
    setStyleSheet("#skInput {background:transparent;}");
    setAttribute(Qt::WA_TranslucentBackground);
#ifdef Q_OS_MACOS
    setWindowFlags(Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Popup);
#endif


    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setGeometry(0, 0, 500, 50);
    setStyleSheet("#skInput {"
                    "background-color: #f6f6f6;"
                    "border-radius: 10px;"
                    "font: 30pt Courier"
                  "}");
    setTextMargins(5, 0, 0, 0);
    setAttribute(Qt::WA_MacShowFocusRect, 0);

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
    setCompleter(completer);
    QAbstractItemView *abstractItemView = completer->popup();


    connect(this, &QLineEdit::returnPressed, this, &MainWindow::EnterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::EnterPressed);
    connect(popup, &FuzzyPopup::popupShow, this, &MainWindow::setAngleCorners);
    connect(popup, &FuzzyPopup::popupHide, this, &MainWindow::setRoundedCorners);

    connect(this, &QLineEdit::textEdited, this, &MainWindow::SearchEvent);

    connect(settingsAcc, &QAction::triggered, this, &MainWindow::showSettings);

    this->activateWindow();
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, (QEvent *)eventFocus, Qt::LowEventPriority);

    QPoint pos(width()-5, 5);
    QMouseEvent e(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(this, &e);
    QMouseEvent f(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(this, &f);

    QWidget::setFocusProxy(this);
    QRect ag = qApp->desktop()->availableGeometry();
    ag.setHeight(ag.height()/2);
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            ag
        )
    );

    lockInput();
}

void MainWindow::handleDataLoad() {
    if (!this->completer()->isDataSet()) {
        //QMessageBox::StandardButton reply;
        int reply = QMessageBox::question(this,
                                      "Error", "Failed to load data from database",
                                      "Quit", "Open Settings");
        qDebug() << "Reply: " << reply;
        if (reply == 0) {
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
        FuzzyCompleter *c = this->completer();
        c->setUp(wordList);
    } catch (etcd::ClientException e) {
        qDebug() << "Exception";
    }
}

void MainWindow::lockInput()
{
    // TODO (dukov) Use gray style here
    setReadOnly(true);
    setText("Loading data...");
}

void MainWindow::unlockInput()
{
    // TODO (dukov) Restore default style
    this->setText("");
    this->setReadOnly(false);

}

void MainWindow::setWriteFd(int fd){
    wfd = fd;
}

void MainWindow::setData(std::string d) {
    data = d;
}

void MainWindow::hideEvent(QHideEvent *e) {
    std::string key = text().toStdString();
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

        write(wfd, val.c_str(), std::strlen(val.c_str()));
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
    FuzzyCompleter *c = completer();
    c->update(text());
}

void MainWindow::showSettings() {
    SKSettings s;
    int r = s.exec();
    qDebug() << "Settings result: " << r;
    if (r == QDialog::Accepted) {
        this->lockInput();
        this->completer()->cleanUp();
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

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.addAction(settingsAcc);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

