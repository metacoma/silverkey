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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow),
    lineEdit(new FuzzyLineEdit(this)),
    settingsAcc(new QAction(tr("&Settings"), this))
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    setObjectName("skForm");
    setStyleSheet("#skForm {background:transparent;}");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    QStringList wordList;

    etcd::Client<example::RapidReply> etcd_client(
                settings.value("server", "nseha.linkpc.net").toString().toStdString(),
                settings.value("port", 22379).toInt());

    example::RapidReply reply = etcd_client.GetAll("/");

    //reply.KvPairs


    //example::RapidReply::GetAll(kvpairs);
    reply.GetAll(kvpairs);

    for (auto iter = kvpairs.begin(); iter != kvpairs.end(); ++iter) {
        //wordList << "hello";
        wordList << QString::fromStdString(iter->first);
    }

    lineEdit->setFocusPolicy(Qt::StrongFocus);
    lineEdit->setFocus();
    lineEdit->setGeometry(0, 0, 300, 30);
    lineEdit->setStyleSheet("QLineEdit {background-color: white;}");


    FuzzyCompleter *completer = new FuzzyCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    QAbstractItemView *abstractItemView = completer->popup();


    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::EnterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::EnterPressed);

    connect(lineEdit, &QLineEdit::textEdited, this, &MainWindow::SearchEvent);

    connect(settingsAcc, &QAction::triggered, this, &MainWindow::showSettings);

    this->activateWindow();
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    qApp->postEvent(this, (QEvent *)eventFocus, Qt::LowEventPriority);

    QPoint pos(lineEdit->width()-5, 5);
    QMouseEvent e(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &e);
    QMouseEvent f(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(lineEdit, &f);

    QWidget::setFocusProxy(this);

    ui->setupUi(this);
}

void MainWindow::setWriteFd(int fd){
    wfd = fd;
}

void MainWindow::setData(std::string d) {
    data = d;
}

void MainWindow::hideEvent(QHideEvent *e) {
    std::string key = lineEdit->text().toStdString();
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
    FuzzyCompleter *c = lineEdit->completer();
    c->update(lineEdit->text());
}

void MainWindow::showSettings() {
    SKSettings s;
    s.exec();
}
#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.addAction(settingsAcc);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

