#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QKeyEvent>

#include "uexception.h"
#include "uglobal.h"

class UGLOBALHOTKEY_EXPORT UKeySequence : public QObject
{
    Q_OBJECT
public:
    explicit UKeySequence(QObject *parent = nullptr);
    explicit UKeySequence(const QString& str, QObject *parent = nullptr);
    void FromString(const QString& str);
    QString ToString();
    void AddKey(int key);
    void AddKey(const QString& key);
    void AddModifiers(Qt::KeyboardModifiers mod);
    void AddKey(const QKeyEvent* event);
    inline size_t Size() const {
        return static_cast<size_t>(Keys.size());
    }
    inline size_t operator [] (size_t n) const {
        if (static_cast<int>(n) > Keys.size()) {
            throw UException("Out of bounds");
        }
        return static_cast<size_t>(Keys[static_cast<int>(n)]);
    }
    QVector<int> GetSimpleKeys() const;
    QVector<int> GetModifiers() const;
private:
    QVector<int> Keys;
};

