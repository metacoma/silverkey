#ifndef DATAINSERTER_H
#define DATAINSERTER_H

#include <QObject>

#include <yaml-cpp/yaml.h>

class DataInserter : public QObject
{
    enum class DataType
    {
        Url,
        Json,
        Yaml,
        WrongType
    };

    Q_OBJECT
public:
    explicit DataInserter(QObject *parent = nullptr);

    Q_INVOKABLE void insertData(const QString &rawData);

private:
    DataType detectType(const QString &data) const;
    void insertFromUrl(const QUrl &url);
    void insertFromYaml(const QString &yaml);
    void insertFromJson(const QString &json);

    void recursiveInsertFromJson(QJsonObject &&object, const QStringList &path);
    void recursiveInsertFromYaml(const YAML::Node &node, const QStringList &path);

    void insertKeyValue(const QString &key, const QVariant &value);
};

#endif // DATAINSERTER_H
