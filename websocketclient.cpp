#include "websocketclient.h"
#include "pict_data/message.qpb.h"
#include <QFileInfo>
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
    if (base.contentField() == pict_data::BaseMessage::ContentFields::ServerResp) {
        const auto &response = base.serverResp();
        QStringList sl;
        qDebug() << "filename" << response.filename() << "imageId" << response.imageId() <<
            "userLogin" << response.content() << "status" << response.status();
//        emit bucketsReceived(sl);
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
    message.setUserLogin("Ivon");

    base.setReqUserBuckets(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
    return {};
}

Q_INVOKABLE int WebSocketClient::deleteImageFromBucketRequest(const QString &filePath){
    QUrl minioUrl(filePath);
    QString path = minioUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    qDebug() << "Path: " << path;
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    QString key = parts.join('/');

    pict_data::BaseMessage base;
    pict_data::DeleteImageRequest message;
    qDebug() << "deleteImageFromBucketRequest" << filePath;

    message.setFilename(key);
    message.setImageId("1111111");
    message.setUserLogin("Ivon");
    message.setBucketName(bucket);

    base.setDeleteImage(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
    return m_webSocket->sendBinaryMessage(data);
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

Q_INVOKABLE int WebSocketClient::addImageRequest(const QString &filePath, const QString &minioPath){
    qDebug() << "addImageRequest(const QStringList &filePath)" << filePath << "minioPath: " << minioPath;
    QString cleanPath = filePath;
    if (cleanPath.startsWith("file:///")) {
        cleanPath = QUrl(cleanPath).toLocalFile();
    }

    // QFile *fil = new QFile(cleanPath);
    // if (!fil->open(QIODevice::ReadOnly)) {
    //     qDebug() << "Could not open file:" << cleanPath;
    //     delete fil;
    //     return -1; // Ошибка открытия файла
    // }
    // else qDebug() << "Open file: " << cleanPath;

    QFileInfo fileInfo(cleanPath);

    QString fileName = fileInfo.fileName();

    QUrl minioUrl(minioPath);
    QString path = minioUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    pict_data::BaseMessage base;
    pict_data::AddImageRequest message;

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    int sz = parts.size();
    QString folder = "";
    if(sz > 2) {
        folder = parts.sliced(1, sz - 2).join("/");
    }
    qDebug() << "bucket: " << bucket << " folder " << folder << "fileName" << fileName << "cleanPath" << cleanPath;
//    QString localPath = QUrl(filePath).toLocalFile();
    // QFile file(cleanPath);
    // if(!file.isOpen()) {
    //     qDebug() << "Error: File " << cleanPath << " is not open. The parameter filePath: " << filePath;
    //     return -5;
    // }
    QFile *file = new QFile(cleanPath);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file:" << cleanPath;
        delete file;
        return -1; // Ошибка открытия файла
    }
    else qDebug() << "Open file: " << cleanPath;
    QByteArray fileData = file->readAll();
    message.setFilename(fileName);
    message.setUserLogin("Ivon");
    message.setBucketName(bucket);
    message.setFolder(folder);
    message.setInfo("jpg");
    message.setData(fileData);

    base.setAddImage(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ //  m_webSocket->sendBinaryMessage(data);
    return m_webSocket->sendBinaryMessage(data);
}
/*

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file";
        return;
    }

    QByteArray fileData = file.readAll();

    pict_data::BaseMessage base;
    pict_data::Picture message;
    message.setFilename(fileInfo.fileName());
    message.setEmailLogin("forever_young");
    message.setData(fileData);
    message.setContentType("image_1");
    message.setTimestamp(QDateTime::currentMSecsSinceEpoch());

    base.setPict(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    qint64 sz = m_webSocket->sendBinaryMessage(data);

*/
