#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

#include <yaml-cpp/yaml.h>

class Requester;
class DataManager : public QObject
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
    explicit DataManager(QObject *parent = nullptr);

    Q_INVOKABLE void insertData(const QString &rawData);
    void requestValue(const QString &key);

signals:
    void wordListUpdated(const QStringList &wordList);
    void wordListLoadError();
    void updateRequestError();
    void writeToServerError();
    void loadFromServerError();
    void valueLoaded(const QString key, const QString &value);
    void valueSaved(const QString key, const QString &value);

private:
    DataType detectType(const QString &data) const;
    void insertFromUrl(const QUrl &url);
    void insertFromYaml(const QString &yaml);
    void insertFromJson(const QString &json);

    void recursiveInsertFromJson(QJsonObject &&object, const QStringList &path);
    void recursiveInsertFromYaml(const YAML::Node &node, const QStringList &path);

    void insertKeyValue(const QString &key, const QVariant &value);

    void requestWordList();
    void requestSaveValue(const QString &key, const QString &value);
    void connectToHost();

    void waitForUpdates();

private:
    Requester *m_httpClient = nullptr;
    QStringList m_wordList;
};

#endif // DATAMANAGER_H
