#ifndef KEYSMODEL_H
#define KEYSMODEL_H

#include <QSortFilterProxyModel>
#include <QStringListModel>

class KeysModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    KeysModel(const QStringList &words, QObject *parent = nullptr);
    Q_INVOKABLE void setSortPattern(const QString &pattern);
    Q_INVOKABLE void setFilter(const QString &filter);

    void append(const QString &key);

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &) const override;

private:
    QString m_pattern;
    QString m_filter;
};

#endif // KEYSMODEL_H