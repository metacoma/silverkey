#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etcd.hpp"
#include "rapid_reply.hpp"
#include <QLineEdit>
#include <QCompleter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QStringList wordList;

    //etcd::Client<example::RapidReply> etcd = new etcd::Client("localhost", 2379); http://nseha.linkpc.net:22379
    etcd::Client<example::RapidReply> etcd_client("nseha.linkpc.net", 22379);
    example::RapidReply reply = etcd_client.GetAll("/");

    //reply.KvPairs
    std::map<std::string, std::string> kvpairs;

    //example::RapidReply::GetAll(kvpairs);
    reply.GetAll(kvpairs);

    for (auto iter = kvpairs.begin(); iter != kvpairs.end(); ++iter) {
        //wordList << "hello";
        wordList << QString::fromStdString(iter->first);
    }


    QLineEdit *lineEdit = new QLineEdit(this);

    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);



    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
