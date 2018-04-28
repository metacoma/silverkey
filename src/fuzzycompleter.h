#ifndef FUZZYCOMPLETER_H
#define FUZZYCOMPLETER_H

#include <QLineEdit>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>

class FuzzySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FuzzySortFilterProxyModel(QObject *parent=0);
    QString sortPattern();
    void setSortPattern(QString pat);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QString pattern;
};



class FuzzyCompleter : public QCompleter
{
    Q_OBJECT

public:
    FuzzyCompleter(const QStringList& words, QObject * parent);
    void update(QString pattern);

private:
    QStringListModel *m_model;
    FuzzySortFilterProxyModel *p_model;
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


#endif // FUZZYCOMPLETER_H