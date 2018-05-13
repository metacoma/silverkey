#include "fuzzycompleter.h"
#include <QLineEdit>
#include <QCompleter>
#include <QDebug>


int compareStr(const std::string& s, const std::string& t) {
    const std::size_t n = t.size();
    const std::size_t m = s.size();
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }
    int deletionCost;
    int insertionCost;
    int substitutionCost;
    std::vector<int> v0(n+1);
    std::vector<int> v1(n+1);


    for (int i=0; i<v0.size(); ++i){
        v0[i] = i;
    }

    for (int i=0; i<m; ++i){
        v1[0] = i+1;
        for (int j=0; j<n; ++j) {
            deletionCost = v0[j+1] + 1;
            insertionCost = v1[j] + 1;
            substitutionCost = tolower(s[i]) == tolower(t[j]) ? v0[j]: v0[j] + 1;

            v1[j+1] = std::min({deletionCost, insertionCost, substitutionCost});
        }
        v1.swap(v0);
    }
    return v0[n];

}

bool sorter(QString a, QString b, QString pat) {
    int lenA = std::max(pat.length(), a.length());
    int lenB = std::max(pat.length(), b.length());
    double x = 1.0 - (double)compareStr(pat.toStdString(),a.toStdString())/lenA;
    double y = 1.0 - (double)compareStr(pat.toStdString(),b.toStdString())/lenB;
    return ( x > y );
}

void FuzzyPopup::showEvent(QShowEvent *event) {
    emit popupShow();
}

void FuzzyPopup::hideEvent(QHideEvent *event) {
    emit popupHide();
}

FuzzySortFilterProxyModel::FuzzySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool FuzzySortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    QVariant leftData = sourceModel()->data(source_left);
    QVariant rightData = sourceModel()->data(source_right);
    return sorter(leftData.toString(),rightData.toString(), pattern);
}

QString FuzzySortFilterProxyModel::sortPattern() {
    return pattern;
}

void FuzzySortFilterProxyModel::setSortPattern(QString pat) {
    pattern = pat;
}

FuzzyCompleter::FuzzyCompleter(QObject *parent)
    : QCompleter(parent)
{
}

FuzzyCompleter::FuzzyCompleter(const QStringList &words, QObject *parent)
    : QCompleter(parent)
{
    setUp(words);
}

void FuzzyCompleter::setUp(const QStringList &words)
{
    m_model = new QStringListModel(words);
    p_model = new FuzzySortFilterProxyModel(this);
    p_model->setSourceModel(m_model);
    setModel(p_model);

}

void FuzzyCompleter::update(QString pattern){
    p_model->setSortPattern(pattern);
    p_model->setDynamicSortFilter(false);
    p_model->sort(0);
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

