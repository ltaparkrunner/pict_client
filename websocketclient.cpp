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
void WebSocketClient::onBinaryMessageReceived(const QByteArray &data) {
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

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent) : QObject(parent)
    , m_webSocket (new QWebSocket())
    , m_url(url)
    , m_path("")
{
    // m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::connectToServer);
    connect(&m_pingTimer, &QTimer::timeout, [&]() {
        if (m_webSocket->state() == QAbstractSocket::ConnectedState) m_webSocket->ping();
    });
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    m_webSocket->setSslConfiguration(sslConf);

    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketClient::onBinaryMessageReceived);
//    connect(m_webSocket, &QWebSocket::connected, this, []() { qDebug() << "Connected!"; });
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::errorOccurred, this, &WebSocketClient::onError);
}

Q_INVOKABLE void WebSocketClient::connectToServer(/*const QString &url*/) {
    m_reconnectTimer.setSingleShot(true);
    m_webSocket->open(m_url);
}

QString WebSocketClient::lastReceivedPath() const { return m_path; }

Q_INVOKABLE QStringList WebSocketClient::getBucketsList() const{
    return {};
}
Q_INVOKABLE QStringList WebSocketClient::getImagesListfromBucket(const QString &bucket) const{
    return {};
}


void WebSocketClient::onConnected() {
    qDebug() << "Подключено. Heartbeat запущен.";
    m_reconnectTimer.stop();
    // m_pingTimer.start(PING_INTERVAL);
}

void WebSocketClient::onDisconnected() {
    qDebug() << "Связь потеряна. Ожидание переподключения...";
//    m_pingTimer.stop();
//    m_reconnectTimer.start(RECONNECT_INTERVAL);
}

// Обработка текста
void WebSocketClient::onTextMessageReceived(const QString &message) {
    qDebug() << "Текст от сервера:" << message;
}


void WebSocketClient::onError(QAbstractSocket::SocketError error) {
    qDebug() << "Ошибка:" << m_webSocket->errorString();
    if (m_webSocket->state() != QAbstractSocket::ConnectedState && !m_reconnectTimer.isActive()) {
        m_reconnectTimer.start(RECONNECT_INTERVAL);
    }
}
