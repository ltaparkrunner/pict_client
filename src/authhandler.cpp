#include "authhandler.h"
#include "pict_data/message.qpb.h"
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

AuthHandler::AuthHandler(/*WebSocketClient *client,*/ QObject *parent)
    : QObject(parent)   //, m_client(client)
    , settings("Alex@Co", "Alex@Co")
    , m_authToken(settings.value("Auth/accessToken", "").toString())
    , m_loggedIn(m_authToken != "")
{
    // Listen to all incoming raw traffic from the websocket client
    // connect(m_client, &WebSocketClient::authResponseReceived,
    //         this, &AuthHandler::handleIncomingAuthData);
}

/*
void AuthHandler::handleIncomingAuthData(const pict_data::AuthResponse &data) {
    // 1. Parse incoming data using Protobuf
    pict_data::AuthResponse response;
    QProtobufSerializer serializer;

    {
        QString err = data.error();
        QString token = data.token();
        qDebug() << "handleIncomingAuthData  Token: " << token << "Response error" << err;
    }
}
*/

/*
Q_INVOKABLE void AuthHandler::registerUser(const QString &login, const QString &password) {
    qDebug() << "Register attempt:" << login << " password: " << password;
    pict_data::ClientEnvelope cenv;
    pict_data::RegisterRequest message;
    message.setUserLogin(login);
    message.setPassword(password);

    cenv.setType(pict_data::ClientEnvelope::Type::AUTH_REQUEST);
    cenv.setRegRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    //qint64 sz =
    m_client->sendBinaryMessage(data);
    return;
}

Q_INVOKABLE void AuthHandler::login(const QString &login, const QString &password) {
    qDebug() << "Login attempt:" << login << " password: " << password;
    pict_data::ClientEnvelope cenv;
    pict_data::AuthRequest message;
    message.setUserLogin(login);
    message.setPassword(password);

    cenv.setType(pict_data::ClientEnvelope::Type::AUTH_REQUEST);
    cenv.setAuthRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    //qint64 sz =
    m_client->sendBinaryMessage(data);
    return;
}
*/

//  "http://localhost:8081/login"
Q_INVOKABLE void AuthHandler::sendLogin(QString user, QString pass) {
//    qDebug() << "AuthHandler::sendLogin" << user << "  " << pass;
    sendAuth(user, pass, "http://localhost:8081/auth/login");
}

//  "http://localhost:8081/register"
Q_INVOKABLE void AuthHandler::sendRegister(QString user, QString pass) {
//    qDebug() << "AuthHandler::sendRegister" << user << "  " << pass;
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
            settings.beginGroup("Auth");
            settings.setValue("accessToken", m_authToken);
            settings.endGroup();
            setLoggedIn(true);
            emit succAuth(reply->errorString());
            emit startWebSocket(/*m_authToken*/);
        }
        else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.object().contains("error")) {
//                qDebug() << "Second branch Get error object : " << doc.object()["error"].toString();
                emit errAuth(doc.object()["error"].toString());
            }
            else emit errAuth(reply->errorString());
        }
        reply->deleteLater();
    });
}

bool AuthHandler::loggedIn() const{
    return m_loggedIn;
}

void AuthHandler::setLoggedIn(bool value) {
    qDebug() << "AuthHandler::setLoggedIn: " << value;
    if (m_loggedIn == value)
        return; // Если статус не изменился, ничего не делаем (защита от зацикливания)

    m_loggedIn = value;
    emit loggedInChanged(); // 4. ВАЖНО: триггерим сигнал, чтобы QML узнал об изменениях
}

Q_INVOKABLE void AuthHandler::logout(){
    qDebug() << "Инициация процесса выхода из системы (Logoff)...";

    // 1. Удаляем токен из постоянной памяти устройства (сохраненная сессия)
    // Используйте те же имена организации и приложения, что и при инициализации
    QSettings settings("MyCompany", "MyApplication");

    if (settings.contains("auth/jwt_token")) {
        settings.remove("auth/jwt_token");
        settings.sync(); // Принудительно сохраняем изменения на диск
        qDebug() << "JWT-токен успешно удален из QSettings.";
    }

    // 2. Очищаем токен из оперативной памяти C++ класса (текущая сессия)
    // Допустим, у вас есть приватная переменная QString m_authToken;
    this->m_authToken = "";

    // 3. (Опционально) Если вы используете WebSockets или TCP-сокеты, закройте их здесь:
    /*
    if (m_webSocket && m_webSocket->state() == QAbstractSocket::ConnectedState) {
        m_webSocket->close(QWebSocketProtocol::CloseCodeNormal, "User logged out");
        qDebug() << "Постоянное соединение WebSocket закрыто.";
    }
    */

    // 4. Меняем статус авторизации на false.
    // Сеттер setLoggedIn(bool) автоматически вызовет emit loggedInChanged(),
    // что заставит QML переключить видимость экранов (скроет главное окно и откроет LoginDialog).
    this->setLoggedIn(false);

    qDebug() << "Пользователь успешно разлогинен. Интерфейс QML уведомлен.";

}
// TODO: reset the token, if attempt to enter on wrong login/password.
// TODO: logoff button
