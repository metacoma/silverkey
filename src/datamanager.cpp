#include "datamanager.h"

#include "requester.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>

#include <exception>

//TODO: !!! Build yaml-cpp for Windows

inline QStringList getKeys(const QJsonObject &object)
{
    QStringList result;
    if (object.value("dir") == QJsonValue::Undefined) {
        result.append(object.value("key").toString());
    } else {
        const QJsonArray nodes = object.value("nodes").toArray();
        for (const auto &node : nodes)
            result += getKeys(node.toObject());
    }
    return result;
}

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    m_httpClient = new Requester(settings.value("server", "silverkey.app").toString(),
                                 settings.value("port", 2379).toInt(), nullptr, this);

    connect(this, &DataManager::updateRequestError, this, &DataManager::waitForUpdates);
    requestWordList();
}

void DataManager::insertData(const QString &rawData)
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

DataManager::DataType DataManager::detectType(const QString &data) const
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

void DataManager::insertFromUrl(const QUrl &url)
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
    QNetworkAccessManager *qnam = new QNetworkAccessManager();
    auto reply = qnam->get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply, qnam]() {
        insertData(reply->readAll());
        delete qnam;
    });
    connect(reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this,
            [this, reply, qnam](QNetworkReply::NetworkError) {
                delete qnam;
                //TODO: handle and emit error
            });
    connect(reply, &QNetworkReply::sslErrors, this, [this, reply]() {
        //TODO: handle and emit error
    });
}

void DataManager::insertFromYaml(const QString &yaml)
{
    try {
        auto nodes = YAML::LoadAll(yaml.toStdString());

        if (nodes.empty()) {
            //TODO: emit error
            return;
        }

        for (const auto &node : nodes) {
            if (node.IsMap())
                recursiveInsertFromYaml(node, {});
        }
    } catch (std::exception *e) {
        //TODO: emit error
    }
}

void DataManager::insertFromJson(const QString &json)
{
    QJsonParseError error;
    auto document = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || document.isNull() || document.isArray() || document.isEmpty()) {
        //TODO: emit error
        return;
    }

    recursiveInsertFromJson(document.object(), {});
}

void DataManager::recursiveInsertFromJson(QJsonObject &&object, const QStringList &path)
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

void DataManager::recursiveInsertFromYaml(const YAML::Node &node, const QStringList &path)
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

void DataManager::insertKeyValue(const QString &key, const QVariant &value)
{
    requestSaveValue(key, value.toString());
}

void DataManager::waitForUpdates()
{
    qDebug() << "Start waiting for DB updates loop";

    m_httpClient->sendRequest("v2/keys/?wait=true&recursive=true", [this](const QJsonObject &) { requestWordList(); },
                              [this](const QJsonObject &) {
                                  qDebug() << "Error: connection dropped";
                                  emit updateRequestError();
                              });
}

void DataManager::requestValue(const QString &key)
{
    auto path = QString("v2/keys%1%2").arg(key.front() != '/' ? "/" : "").arg(key);

    m_httpClient->sendRequest(path,
                              [this, key](const QJsonObject &object) {
                                  auto value = object.value("node").toObject().value("value").toString();
                                  qDebug() << "Got value" << value << "for key" << key;
                                  emit valueLoaded(key.front() != '/' ? "/" + key : key, value);
                              },
                              [this](const QJsonObject &) { emit loadFromServerError(); });
}

void DataManager::requestSaveValue(const QString &key, const QString &value)
{
    auto path = QString("v2/keys%1%2").arg(key.front() != '/' ? "/" : "").arg(key);

    auto encodedVal = QString(QUrl::toPercentEncoding(value));
    m_httpClient->sendRequest(path,
                              [this, key](const QJsonObject &object) {
                                  QString value = object.value("node").toObject().value("value").toString();
                                  qDebug() << "Successfully written data" << value << "for key" << key;
                                  emit valueSaved(key.front() != '/' ? "/" + key : key, value);
                              },
                              [this](const QJsonObject &) { emit writeToServerError(); }, Requester::Type::PUT,
                              QString("value=%1").arg(encodedVal));
}

void DataManager::requestWordList()
{
    m_httpClient->sendRequest("v2/keys/?recursive=true&sorted=true",
                              [this](const QJsonObject &object) {
                                  if (!object.isEmpty() && object.contains("node")) {
                                      m_wordList = getKeys(object.value("node").toObject());
                                      qDebug() << "Got obj" << m_wordList.join(" ");
                                      emit wordListUpdated(m_wordList);
                                  } else {
                                      emit wordListLoadError();
                                  }
                              },
                              [this](const QJsonObject &) { emit wordListLoadError(); });
}
