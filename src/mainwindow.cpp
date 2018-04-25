#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etcd.hpp"
#include "rapid_reply.hpp"

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>



int compareStr(std::string s, std::string t) {
    if (t.length() == 0 ) {
        return s.length();
    }
    if (s.length() == 0) {
        return t.length();
    }

    int n = t.length();
    int m = s.length();
    int deletionCost;
    int insertionCost;
    int substitutionCost;
    std::vector<int> v0(n+1);
    std::vector<int> v1(n+1);


    for (int i=0; i<n+1; ++i){
        v0[i] = i;
    }

    for (int i=1; i<m+1; ++i){
        v1[i] = i+1;
        for (int j=1; j<n+1; ++j) {
            deletionCost = v0[j] + 1;
            insertionCost = v1[j-1] + 1;
            substitutionCost = s[i-1] == t[j-1] ? v0[j-1]: v0[j-1] + 1;

            v1[j] = std::min({deletionCost, insertionCost, substitutionCost});
        }
        v0.swap(v1);
    }
    qDebug() << t.c_str() << v1[n+1];
    return v1[n+1];

}

bool sorter(QString a, QString b, QString pat) {
    return (compareStr(pat.toStdString(),a.toStdString()) > compareStr(pat.toStdString(),b.toStdString()));
}

void FuzzyCompleter::update(QString pattern){
    QStringList weighted = m_list.filter(pattern, caseSensitivity());;
    m_model.setStringList(weighted);
    m_word = pattern;
    complete();

}

FuzzyLineEdit::FuzzyLineEdit(QWidget *parent)
    : QLineEdit(parent), c(0)
{
}

FuzzyLineEdit::~FuzzyLineEdit()
{
}

FuzzyCompleter *FuzzyLineEdit::completer() const {
    return c;
}

void FuzzyLineEdit::setCompleter(FuzzyCompleter *completer) {
    if (c) {
        disconnect(c, 0, this, 0);
        c->setWidget(0);
        if (c->parent() == this) {
            delete c;
        }
    }
    c = completer;

    if (!c) {
        return;
    }

    if (c->widget() == 0) {
        c->setWidget(this);
    }

    QObject::connect(this->completer(), SIGNAL(activated(QString)),
                     this, SLOT(setText(QString)));
    QObject::connect(this->completer(), SIGNAL(highlighted(QString)),
                     this, SLOT(setText(QString)));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow),
    lineEdit(new FuzzyLineEdit(this))
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


    FuzzyCompleter *completer = new FuzzyCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    QAbstractItemView *abstractItemView = completer->popup();


    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::EnterPressed);
    connect(abstractItemView, &QAbstractItemView::clicked, this, &MainWindow::EnterPressed);

    connect(lineEdit, &QLineEdit::textEdited, this, &MainWindow::SearchEvent);

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

void MainWindow::hideEvent(QHideEvent *e) {

    std::string val = kvpairs[lineEdit->text().toUtf8().constData()];
    qDebug() << "Hide action, value is " << QString(val.c_str());

    write(wfd, val.c_str(), std::strlen(val.c_str()));
    e->accept();
    qApp->closeAllWindows();


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

