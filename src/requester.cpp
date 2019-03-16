/* Code is taken from https://github.com/Bragaman/QtRestApiRequester
 * Many thanks to Dmitry Bragin for REST client implementation
 */

#include "requester.h"

static const QString HTTP_TEMPLATE = "http://%1:%2/%3";
static const QString HTTPS_TEMPLATE = "https://%1:%2/api/%3";
static const QString KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
static const QString KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";

Requester::Requester(const QString &host, int port, QSslConfiguration *sslConfiguration, QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    updateConfig(host, port, sslConfiguration);
}

void Requester::updateConfig(const QString &host, int port, QSslConfiguration *sslConfiguration)
{
    m_host = host;
    m_port = port;
    m_sslConfig = sslConfiguration;
    m_pathTemplate = m_sslConfig ? HTTPS_TEMPLATE : HTTP_TEMPLATE;
}

void Requester::sendRequest(const QString &apiString, const handleFunction &funcSuccess,
                            const handleFunction &funcError, Requester::Type type, const QString &data)
{
    QNetworkRequest request = createRequest(apiString);

    QNetworkReply *reply = nullptr;
    switch (type) {
    case Type::POST:
        reply = m_manager->post(request, data.toUtf8());
        break;

    case Type::GET:
        reply = m_manager->get(request);
        break;

    case Type::DELETE:
        reply = data.isEmpty() ? m_manager->deleteResource(request)
                               : sendCustomRequest(m_manager, request, "DELETE", data);
        break;

    case Type::PATCH:
        reply = sendCustomRequest(m_manager, request, "PATCH", data);
        break;

    case Type::PUT:
        reply = m_manager->put(request, QString("%1;").arg(data).toUtf8());
        break;
    }

    connect(reply, &QNetworkReply::finished, this, [this, funcSuccess, funcError, reply]() {
        QJsonObject object = parseReply(reply);

        if (onFinishRequest(reply)) {
            if (funcSuccess != nullptr)
                funcSuccess(object);
        } else {
            if (funcError != nullptr) {
                handleQtNetworkErrors(reply, object);
                funcError(object);
            }
        }
        reply->close();
        reply->deleteLater();
    });
}

void Requester::sendMulishGetRequest(const QString &apiString, const handleFunction &funcSuccess,
                                     const handleFunction &funcError, const finishFunction &funcFinish)
{
    QNetworkRequest request = createRequest(apiString);
    qDebug() << "GET REQUEST " << request.url().toString() << "\n";
    auto reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, funcSuccess, funcError, funcFinish, reply]() {
        QJsonObject object = parseReply(reply);
        if (onFinishRequest(reply)) {
            if (funcSuccess != nullptr)
                funcSuccess(object);
            QString nextPage = object.value("next").toString();
            if (!nextPage.isEmpty()) {
                QStringList apiMethodWithPage = nextPage.split("api/");
                sendMulishGetRequest(apiMethodWithPage.value(1), funcSuccess, funcError, funcFinish);
            } else {
                if (funcFinish != nullptr)
                    funcFinish();
            }
        } else {
            handleQtNetworkErrors(reply, object);
            if (funcError != nullptr)
                funcError(object);
        }
        reply->close();
        reply->deleteLater();
    });
}

QString Requester::token() const
{
    return m_token;
}

void Requester::setToken(const QString &value)
{
    m_token = value;
}

QByteArray Requester::variantMapToJson(const QVariantMap &data)
{
    QJsonDocument postDataDoc = QJsonDocument::fromVariant(data);
    QByteArray postDataByteArray = postDataDoc.toJson();

    return postDataByteArray;
}

QNetworkRequest Requester::createRequest(const QString &apiString)
{
    QNetworkRequest request;
    QString url = m_pathTemplate.arg(m_host).arg(m_port).arg(apiString);
    request.setUrl(QUrl(url));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Accept", "*/*");
    if (!m_token.isEmpty())
        request.setRawHeader("Authorization", QString("token %1").arg(m_token).toUtf8());
    if (m_sslConfig != nullptr)
        request.setSslConfiguration(*m_sslConfig);

    return request;
}

QNetworkReply *Requester::sendCustomRequest(QNetworkAccessManager *manager, QNetworkRequest &request,
                                            const QString &type, const QString &data)
{
    request.setRawHeader("HTTP", type.toUtf8());
    QByteArray postDataByteArray;
    postDataByteArray.append(data);
    QBuffer *buffer = new QBuffer;
    buffer->setData(postDataByteArray);
    buffer->open(QIODevice::ReadOnly);
    QNetworkReply *reply = manager->sendCustomRequest(request, type.toUtf8(), buffer);
    buffer->setParent(reply);
    return reply;
}

QJsonObject Requester::parseReply(QNetworkReply *reply)
{
    QJsonObject jsonObject;
    QJsonDocument jsonDocument;
    QJsonParseError parseError;
    auto replyText = reply->readAll();
    jsonDocument = QJsonDocument::fromJson(replyText, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << replyText;
        qWarning() << "Json parse error: " << parseError.errorString();
    } else {
        if (jsonDocument.isObject())
            jsonObject = jsonDocument.object();
        else if (jsonDocument.isArray())
            jsonObject["non_field_errors"] = jsonDocument.array();
    }
    return jsonObject;
}

bool Requester::onFinishRequest(QNetworkReply *reply)
{
    auto replyError = reply->error();
    if (replyError == QNetworkReply::NoError) {
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((code >= 200) && (code < 300))
            return true;
    }
    return false;
}

void Requester::handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &object)
{
    auto replyError = reply->error();
    if (!(replyError == QNetworkReply::NoError || replyError == QNetworkReply::ContentNotFoundError
          || replyError == QNetworkReply::ContentAccessDenied
          || replyError == QNetworkReply::ProtocolInvalidOperationError)) {
        qDebug() << replyError;
        object[KEY_QNETWORK_REPLY_ERROR] = reply->errorString();
    } else if (replyError == QNetworkReply::ContentNotFoundError) {
        object[KEY_CONTENT_NOT_FOUND] = reply->errorString();
    }
}
