#include "websocketclient.h"
#include "pict_data/message.qpb.h"
#include <QtProtobuf/QProtobufSerializer>

// void WebSocketClient::onBinaryMessage(const QByteArray &message) {
//     // Десериализация Protobuf
//     MyProtoMessage proto;
//     if (proto.ParseFromArray(message.data(), message.size())) {
//         m_path = QString::fromStdString(proto.path()); // Предположим, в прото есть поле 'path'
//         emit pathReceived(m_path);
//     }
// }
void WebSocketClient::onBinaryMessage(const QByteArray &data) {
    //    qDebug() << "ImageClient::onBinaryMessageReceived(const QByteArray &data)";
    pict_data::BaseMessage base;
    QProtobufSerializer serializer;
    if (!base.deserialize(&serializer, data)) return;
    if (base.contentField() == pict_data::BaseMessage::ContentFields::ListResponse) {
        const auto &response = base.listResponse();
        // for (const auto &info : response.images()) {
        //     downloadImage(info.url());
        // }
    }
}

WebSocketClient::WebSocketClient(QObject *parent) : QObject(parent) {
    // Настройка TLS
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    m_webSocket.setSslConfiguration(sslConf);

    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketClient::onBinaryMessage);
    connect(&m_webSocket, &QWebSocket::connected, this, []() { qDebug() << "Connected!"; });
}

Q_INVOKABLE void WebSocketClient::connectToServer(const QString &url) {
    m_webSocket.open(QUrl(url));
}


QString WebSocketClient::lastReceivedPath() const { return m_path; }
