/* Code is taken from https://github.com/Bragaman/QtRestApiRequester
 * Many thanks to Dmitry Bragin for REST client implementation
 */

#ifndef REQUESTER_H
#define REQUESTER_H

#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

#include <functional>

class Requester : public QObject
{
    Q_OBJECT
public:
    typedef std::function<void(const QJsonObject &)> handleFunction;
    typedef std::function<void()> finishFunction;

    enum class Type
    {
        POST,
        PUT,
        GET,
        PATCH,
        DELETE
    };

    explicit Requester(const QString &host, int port, QSslConfiguration *sslConfiguration, QObject *parent = nullptr);

    void updateConfig(const QString &host, int port, QSslConfiguration *sslConfiguration);

    void sendRequest(const QString &apiStr, const handleFunction &funcSuccess, const handleFunction &funcError,
                     Type type = Type::GET, const QString &data = "");

    void sendMulishGetRequest(const QString &apiString, const handleFunction &funcSuccess,
                              const handleFunction &funcError, const finishFunction &funcFinish);

    QString token() const;
    void setToken(const QString &value);

Q_SIGNALS:
    void networkError();

private:
    QByteArray variantMapToJson(const QVariantMap &data);
    QNetworkRequest createRequest(const QString &apiString);
    QNetworkReply *sendCustomRequest(QNetworkAccessManager *m_manager, QNetworkRequest &request, const QString &type,
                                     const QString &data);
    QJsonObject parseReply(QNetworkReply *reply);
    bool onFinishRequest(QNetworkReply *reply);
    void handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &object);

private:
    QString m_host;
    int m_port;
    QString m_token;
    QSslConfiguration *m_sslConfig = nullptr;

    QString m_pathTemplate;
    QByteArray m_dataByteArray;

    QNetworkAccessManager *m_manager = nullptr;
};

#endif // REQUESTER_H
