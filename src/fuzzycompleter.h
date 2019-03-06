#ifndef FUZZYCOMPLETER_H
#define FUZZYCOMPLETER_H

#include <QLineEdit>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QListView>

class FuzzyPopup : public QListView
{
    Q_OBJECT
private:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

Q_SIGNALS:
    void popupShow();
    void popupHide();
};

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
    FuzzyCompleter(QObject * parent);
    void setUp(const QStringList &words);
    void cleanUp();
    void update(QString pattern);
    bool isDataSet();

private:
    bool dataSet = false;
    QStringListModel *m_model;
    FuzzySortFilterProxyModel *p_model;
};

class FuzzyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FuzzyLineEdit(QWidget *parent = nullptr);
    ~FuzzyLineEdit();

    void setCompleter(FuzzyCompleter *c);
    FuzzyCompleter *completer() const;
    QString getSelectedItem() const;

public Q_SLOTS:
    void setSelectedItem(const QString &value);

Q_SIGNALS:
    void hideApp();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    FuzzyCompleter *c = nullptr;
    QString selectedItem;
};


#endif // FUZZYCOMPLETER_H
