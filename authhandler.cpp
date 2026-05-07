#include "authhandler.h"
#include "pict_data/message.qpb.h"

AuthHandler::AuthHandler(WebSocketClient *client, QObject *parent)
    : QObject(parent), m_client(client)
{
    // Listen to all incoming raw traffic from the websocket client
    // connect(m_client, &WebSocketClient::authResponseReceived,
    //         this, &AuthHandler::handleIncomingNetworkData);
}

void AuthHandler::handleIncomingNetworkData(const QByteArray &data) {
    // 1. Parse incoming data using Protobuf
    pict_data::AuthResponse response;
    QProtobufSerializer serializer;
    if (!response.deserialize(&serializer, data)) {
        qDebug() << "handleIncomingNetworkData Response error";
        return;
    }
    /* if (response.ParseFromArray(data.constData(), data.size()))*/ {

        // 2. Check if this specific response is an Auth message
        QString err = response.error();
        QString token = response.token();
        // if (response.setError()  .has_status()) {
        //     if (response.status() == pict_data::AuthResponse::SUCCESS) {
        //         emit loginSuccess();
        //     } else {
        //         emit loginFailed(QString::fromStdString(response.error_message()));
        //     }
        // }
        qDebug() << "handleIncomingNetworkData  Token: " << token << "Response error" << err;
    }
//    else qDebug() << "Response error";
}

Q_INVOKABLE void AuthHandler::registerUser(const QString &login, const QString &password) {
    qDebug() << "Register attempt:" << login << " password: " << password;
    pict_data::ClientEnvelope cenv;
    pict_data::RegisterRequest message;
    message.setUserLogin(login);
    message.setPassword(password);

    cenv.setType(pict_data::ClientEnvelope::Type::LOGIN_REQUEST);
    cenv.setRegRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return;
}

Q_INVOKABLE void AuthHandler::login(const QString &login, const QString &password) {
    qDebug() << "Login attempt:" << login << " password: " << password;
    pict_data::ClientEnvelope cenv;
    pict_data::AuthRequest message;
    message.setUserLogin(login);
    message.setPassword(password);

    cenv.setType(pict_data::ClientEnvelope::Type::LOGIN_REQUEST);
    cenv.setAuthRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return;
}
