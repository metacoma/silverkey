#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etcd.hpp"
#include "rapid_reply.hpp"

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow),
    lineEdit(new QLineEdit(this))
{
    setStyleSheet("background:transparent;");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    QStringList wordList;

    etcd::Client<example::RapidReply> etcd_client("nseha.linkpc.net", 22379);

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


    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    QAbstractItemView *abstractItemView = completer->popup();


    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::EnterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::EnterPressed);

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

void MainWindow::hideEvent(QHideEvent *e) {
    qDebug() << "Hide";
    QCompleter *none;
    lineEdit->setCompleter(none);
    this->close();
    //this->~MainWindow();
}


void MainWindow::EnterPressed() {
    //this->~MainWindow();
    qDebug() << "EnterPressed";
    this->hide();
}

MainWindow::~MainWindow()
{
    delete lineEdit;
    delete ui;
}
