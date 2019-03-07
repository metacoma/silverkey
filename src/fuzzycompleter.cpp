#include "fuzzycompleter.h"

#include <QDebug>
#include <QKeyEvent>

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

void FuzzyPopup::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    emit popupShow();
}

void FuzzyPopup::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    emit popupHide();
}

FuzzySortFilterProxyModel::FuzzySortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{}

bool FuzzySortFilterProxyModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    QVariant leftData = sourceModel()->data(sourceLeft);
    QVariant rightData = sourceModel()->data(sourceRight);
    return sorter(leftData.toString(), rightData.toString(), m_pattern);
}

QString FuzzySortFilterProxyModel::sortPattern()
{
    return m_pattern;
}

void FuzzySortFilterProxyModel::setSortPattern(const QString &pattern)
{
    m_pattern = pattern;
}

FuzzyCompleter::FuzzyCompleter(QObject *parent) : QCompleter(parent)
{
    m_innerModel = new QStringListModel();
    m_model = new FuzzySortFilterProxyModel(this);
    m_model->setSourceModel(m_innerModel);
    setModel(m_model);
}

void FuzzyCompleter::cleanUp()
{
    qDebug() << "Model cleanup executed";
    m_innerModel->setStringList(QStringList{});
    m_dataSet = false;
}

void FuzzyCompleter::setUp(const QStringList &words)
{
    m_innerModel->setStringList(words);
    m_dataSet = true;
}

void FuzzyCompleter::update(const QString &pattern)
{
    m_model->setSortPattern(pattern);
    m_model->setDynamicSortFilter(false);
    m_model->sort(0);
    complete();
}

bool FuzzyCompleter::isDataSet() const
{
    return m_dataSet;
}

FuzzyLineEdit::FuzzyLineEdit(QWidget *parent) : QLineEdit(parent), m_completer(nullptr)
{}

FuzzyLineEdit::~FuzzyLineEdit()
{}

FuzzyCompleter *FuzzyLineEdit::completer() const
{
    return m_completer;
}

QString FuzzyLineEdit::selectedItem() const
{
    return m_selectedItem;
}

void FuzzyLineEdit::setSelectedItem(const QString &value)
{
    m_selectedItem = value;
}

void FuzzyLineEdit::setCompleter(FuzzyCompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer, nullptr, this, nullptr);
        m_completer->setWidget(nullptr);
        delete m_completer;
    }

    m_completer = completer;

    if (!m_completer)
        return;

    if (m_completer->widget() == nullptr)
        m_completer->setWidget(this);

    QObject::connect(m_completer, qOverload<const QString &>(&FuzzyCompleter::activated), this,
                     &FuzzyLineEdit::setSelectedItem);
    QObject::connect(m_completer, qOverload<const QString &>(&FuzzyCompleter::highlighted), this,
                     &FuzzyLineEdit::setSelectedItem);
}

void FuzzyLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        setSelectedItem("");
        emit hideApplication();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}
