#include "authhandler.h"
//  #include "pict_data/message.qpb.h"
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

AuthHandler::AuthHandler(/*WebSocketClient *client,*/ QObject *parent)
    : QObject(parent)   //, m_client(client)
    , settings("Alex@Co", "Alex@Co")
    , m_authToken(settings.value("Auth/accessToken", "").toString())  // the second argument is a default value
    , m_username(settings.value("Auth/username", "").toString())
    , m_loggedIn(m_authToken != "")
{
    // Listen to all incoming raw traffic from the websocket client
    // connect(m_client, &WebSocketClient::authResponseReceived,
    //         this, &AuthHandler::handleIncomingAuthData);
}

//  "http://localhost:8081/login"
Q_INVOKABLE void AuthHandler::sendLogin(QString user, QString pass) {
    sendAuth(user, pass, "http://localhost:8081/auth/login");
}

//  "http://localhost:8081/register"
Q_INVOKABLE void AuthHandler::sendRegister(QString user, QString pass) {
    sendAuth(user, pass, "http://localhost:8081/auth/register");
//    sendAuth(user, pass, "http://localhost:8081/register");
}

Q_INVOKABLE void AuthHandler::sendAuth(QString user, QString pass, QString path) {
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QUrl url(path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = user;
    json["password"] = pass;

    QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            m_authToken = doc.object().value("token").toString();
            m_username = user;
            settings.beginGroup("Auth");
            settings.setValue("accessToken", m_authToken);
            settings.setValue("username", m_username);
            settings.endGroup();
//            setLoggedIn(true);
//            emit succAuth(reply->errorString());
            emit startWebSocket(/*m_authToken*/);
        }
        else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.object().contains("error")) {
                emit errAuth(doc.object()["error"].toString());
            }
            else emit errAuth(reply->errorString());
        }
        reply->deleteLater();
    });
}

Q_INVOKABLE void AuthHandler::logout(){
    qDebug() << "Инициация процесса выхода из системы (Logoff)...";

    QSettings settings("Alex@Co", "Alex@Co");

    if (settings.contains("Auth/accessToken")) {
        settings.remove("Auth/accessToken");
        settings.sync(); // Принудительно сохраняем изменения на диск
        qDebug() << "JWT-токен успешно удален из QSettings.";
    }
    if (settings.contains("Auth/username")) {
        settings.remove("Auth/username");
        settings.sync(); // Принудительно сохраняем изменения на диск
    }

    this->m_authToken = "";
    this->m_username = "";

    emit this ->logoffSuccess();
    qDebug() << "Пользователь успешно разлогинен. Интерфейс QML уведомлен.";

}
// TODO: reset the token, if attempt to enter on wrong login/password.
// TODO: logoff button
void AuthHandler::setUsername(const QString &newUsername) {
    if (m_username == newUsername)
        return; // Если имя не изменилось, ничего не делаем

    m_username = newUsername;
    emit usernameChanged(); // КРИТИЧЕСКИ ВАЖНО: уведомляем QML об изменении
}

void AuthHandler::onWssConnected(){
    qDebug() << "AuthHandler::onWssConnected()";
//    setLoggedIn(true);
//    emit loggedInChanged();
    emit usernameChanged();
}

void AuthHandler::onWssDisconnected(){
    qDebug() << "AuthHandler::onWssDisconnected()";
//    setLoggedIn(false);
//    emit usernameChanged();
//    emit loggedInChanged();
}
