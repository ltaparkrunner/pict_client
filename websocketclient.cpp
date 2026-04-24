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
        QList<QStringList> sl;
        for (const auto &info : response.images()) {
            sl.append({info.filename(), info.url()});
        }
        if(sreq == usmodel) emit pathsReceived(sl);
        else emit pathsReceived2(sl);
    }
    if (base.contentField() == pict_data::BaseMessage::ContentFields::Buckets) {
        const auto &response = base.buckets();
        QStringList sl;
        for (const auto &info : response.bucketInf()) {
            sl.append(info.bucketName());
            qDebug() << "info.bucketName()" << info.bucketName();
            sl.append(info.url());
            qDebug() << "info.url()" << info.url();
        }
        emit bucketsReceived(sl);
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

Q_INVOKABLE QStringList WebSocketClient::getBucketsListRequest() const{
    pict_data::BaseMessage base;
    pict_data::BucketRequest message;
    message.setUserId("Ivon");

    base.setUser(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
    return {};
}

/*Q_INVOKABLE*/ void WebSocketClient::getImagesListfromBucketRequest(const QString &bucket, sourceReq sr){
    pict_data::BaseMessage base;
    pict_data::ImageListRequest message;
    sreq = sr;
    message.setCount(6);
    message.setBucketName(bucket);

    base.setListRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
}

void WebSocketClient::onConnected() {
    qDebug() << "Connected. Heartbeat has started.";
    m_reconnectTimer.stop();
    m_pingTimer.start(PING_INTERVAL);
}

void WebSocketClient::onDisconnected() {
    qDebug() << "Connection is lost. Waiting for reconnection...";
    m_pingTimer.stop();
//    m_reconnectTimer.start(RECONNECT_INTERVAL);
}

void WebSocketClient::onTextMessageReceived(const QString &message) {
    qDebug() << "Text from server:" << message;
}

void WebSocketClient::onError(QAbstractSocket::SocketError error) {
    qDebug() << "Error:" << m_webSocket->errorString();
    if (m_webSocket->state() != QAbstractSocket::ConnectedState && !m_reconnectTimer.isActive()) {
        m_reconnectTimer.start(RECONNECT_INTERVAL);
    }
}
