#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDebug>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QStringListModel>

#define KEYBOARD_SPEED 20

namespace Ui {
class MainWindow;
class FuzzyCompleter;
class FuzzyLineEdit;
}

class FuzzyCompleter : public QCompleter
{
    Q_OBJECT

public:
    inline FuzzyCompleter(const QStringList& words, QObject * parent) :
            QCompleter(parent), m_list(words), m_model()
    {
        setModel(&m_model);
    }

    inline QString word()
    {
        return m_word;
    }

public:
    void update(QString pattern);

private:
    QStringList m_list;
    QStringListModel m_model;
    QString m_word;
};

class FuzzyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FuzzyLineEdit(QWidget *parent = 0);
    ~FuzzyLineEdit();

    void setCompleter(FuzzyCompleter *c);
    FuzzyCompleter *completer() const;


private:
    FuzzyCompleter *c;
};

class MainWindow : public QMainWindow
{

public:
    explicit MainWindow(QWidget *parent = 0);
    void EnterPressed();
    void EndOfWorkflow();
    void SearchEvent();
    void hideEvent(QHideEvent *e);
    void setWriteFd(int fd);

private:
    Q_OBJECT
    Ui::MainWindow *ui;
    FuzzyLineEdit *lineEdit;
    std::map<std::string, std::string> kvpairs;
    int wfd;
};

#endif // MAINWINDOW_H
