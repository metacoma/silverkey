#ifndef FUZZYCOMPLETER_H
#define FUZZYCOMPLETER_H

#include <QCompleter>
#include <QLineEdit>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QStringListModel>

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
    FuzzySortFilterProxyModel(QObject *parent = nullptr);
    QString sortPattern();
    void setSortPattern(const QString &pattern);

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;

private:
    QString m_pattern;
};

class FuzzyCompleter : public QCompleter
{
    Q_OBJECT
public:
    FuzzyCompleter(QObject *parent = nullptr);
    void setUp(const QStringList &words);
    void cleanUp();
    void update(const QString &pattern);
    bool isDataSet() const;

private:
    bool m_dataSet = false;
    QStringListModel *m_innerModel;
    FuzzySortFilterProxyModel *m_model;
};

class FuzzyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    FuzzyLineEdit(QWidget *parent = nullptr);
    ~FuzzyLineEdit();

    void setCompleter(FuzzyCompleter *completer);
    FuzzyCompleter *completer() const;
    QString selectedItem() const;

public Q_SLOTS:
    void setSelectedItem(const QString &value);

Q_SIGNALS:
    void hideApplication();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    FuzzyCompleter *m_completer = nullptr;
    QString m_selectedItem;
};

#endif // FUZZYCOMPLETER_H
