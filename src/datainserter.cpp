#include "datainserter.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

//TODO: !!! Build yaml-cpp for Windows
DataInserter::DataInserter(QObject *parent) : QObject(parent)
{}

void DataInserter::insertData(const QString &rawData)
{
    auto type = detectType(rawData);
    switch (type) {
    case DataType::Url:
        insertFromUrl(QUrl(rawData.trimmed().split('\n').at(0)));
        break;
    case DataType::Json:
        insertFromJson(rawData);
        break;
    case DataType::Yaml:
        insertFromYaml(rawData);
        break;
    default:
        //TODO: emit error;
        break;
    }
}

DataInserter::DataType DataInserter::detectType(const QString &data) const
{
    if (data.isEmpty())
        return DataType::WrongType;

    if (data.front() == '{' || data.front() == '[')
        return DataType::Json;

    if (data.startsWith("http://") || data.startsWith("https://") || data.startsWith("ftp://")
        || data.startsWith("file://"))
        return DataType::Url;

    return DataType::Yaml;
}

void DataInserter::insertFromUrl(const QUrl &url)
{
    if (url.scheme() == "file") {
        QFile file(url.toLocalFile());
        if (!file.open(QFile::ReadOnly)) {
            //TODO: emit error
            return;
        }
        insertData(file.readAll());
        return;
    }
    QNetworkAccessManager qnam;
    auto reply = qnam.get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() { insertData(reply->readAll()); });
    connect(reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this,
            [this, reply](QNetworkReply::NetworkError) {
                //TODO: handle and emit error
            });
    connect(reply, &QNetworkReply::sslErrors, this, [this, reply]() {
        //TODO: handle and emit error
    });
}

void DataInserter::insertFromYaml(const QString &yaml)
{
    auto nodes = YAML::LoadAll(yaml.toStdString());
    if (nodes.empty()) {
        //TODO: emit error
        return;
    }

    for (const auto &node : nodes) {
        if (node.IsMap())
            recursiveInsertFromYaml(node, {});
    }
}

void DataInserter::insertFromJson(const QString &json)
{
    QJsonParseError error;
    auto document = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || document.isNull() || document.isArray() || document.isEmpty()) {
        //TODO: emit error
        return;
    }

    recursiveInsertFromJson(document.object(), {});
}

void DataInserter::recursiveInsertFromJson(QJsonObject &&object, const QStringList &path)
{
    for (auto it = object.begin(); it != object.end(); ++it) {
        //NOTE: all array items will be ignored
        if (it.value().isArray() || it.value().isNull())
            continue;
        auto newPath = path;
        newPath << it.key();

        if (it.value().isObject())
            recursiveInsertFromJson(it.value().toObject(), newPath);
        else
            insertKeyValue(newPath.join('/'), it.value().toVariant());
    }
}

void DataInserter::recursiveInsertFromYaml(const YAML::Node &node, const QStringList &path)
{
    for (auto it = node.begin(); it != node.end(); ++it) {
        //NOTE: all array items will be ignored
        if ((*it).IsSequence() || (*it).IsNull())
            continue;
        auto newPath = path;
        newPath << QString::fromStdString(node.Tag());

        if ((*it).IsMap())
            recursiveInsertFromYaml((*it), newPath);
        else
            insertKeyValue(newPath.join('/'), QString::fromStdString(node.Scalar()));
    }
}

void DataInserter::insertKeyValue(const QString &key, const QVariant &value)
{
    //TODO: send key:value to server
}
