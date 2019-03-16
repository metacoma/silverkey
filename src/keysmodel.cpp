#include "keysmodel.h"

int compareStrings(const QString &s1, const QString &s2)
{
    const auto s1Length = s1.length();
    const auto s2Length = s2.length();
    if (s2Length == 0)
        return s1Length;

    if (s1Length == 0)
        return s2Length;

    int deletionCost = 0;
    int insertionCost = 0;
    int substitutionCost = 0;
    QVector<int> v1(s2Length + 1);
    QVector<int> v2(s2Length + 1);

    for (int i = 0; i < v1.size(); ++i) {
        v1[i] = i;
    }

    for (int i = 0; i < s1Length; ++i) {
        v2[0] = i + 1;
        for (int j = 0; j < s2Length; ++j) {
            deletionCost = v1[j + 1] + 1;
            insertionCost = v2[j] + 1;
            substitutionCost = s1[i].toLower() == s2[j].toLower() ? v1[j] : v1[j] + 1;

            v2[j + 1] = std::min({deletionCost, insertionCost, substitutionCost});
        }
        v2.swap(v1);
    }
    return v1[s2Length];
}

bool sorter(QString s1, QString s2, QString pattern)
{
    int s1Length = qMax(pattern.length(), s1.length());
    int s2Length = qMax(pattern.length(), s2.length());
    double delta1 = 1.0 - (1.0 * compareStrings(pattern, s1)) / s1Length;
    double delta2 = 1.0 - (1.0 * compareStrings(pattern, s2)) / s2Length;
    return (delta1 > delta2);
}

KeysModel::KeysModel(const QStringList &words, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(new QStringListModel(words, this));
}

bool KeysModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    QVariant leftData = sourceModel()->data(sourceLeft);
    QVariant rightData = sourceModel()->data(sourceRight);
    return sorter(leftData.toString(), rightData.toString(), m_pattern);
}

bool KeysModel::filterAcceptsRow(int sourceRow, const QModelIndex &) const
{
    auto i = sourceModel()->index(sourceRow, 0);
    auto data = sourceModel()->data(i);
    return data.toString().startsWith(m_filter);
}

void KeysModel::setSortPattern(const QString &pattern)
{
    m_pattern = pattern;
}

void KeysModel::setFilter(const QString &filter)
{
    m_filter = filter;
    invalidateFilter();
}
