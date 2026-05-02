#include "websocketclient.h"
#include "pict_data/message.qpb.h"
#include <QFileInfo>
#include <QtProtobuf/QProtobufSerializer>

void WebSocketClient::onBinaryMessageReceived(const QByteArray &data) {
    //    qDebug() << "FileClient::onBinaryMessageReceived(const QByteArray &data)";
    pict_data::BaseMessage base;
    QProtobufSerializer serializer;
    if (!base.deserialize(&serializer, data)) return;
    if (base.contentField() == pict_data::BaseMessage::ContentFields::ListResponse) {
        const auto &response = base.listResponse();
        QList<QStringList> sl;
        for (const auto &info : response.files()) {
            sl.append({info.fileName(), info.url(), "file", info.mongoId()});
            qDebug() << "fileName: " << info.fileName() << "  url: " << info.url() <<
                "  mongoId: " << info.mongoId() << "  ";
        }
        for (const auto &info : response.folders()) {
            sl.append({info.folderName(), info.url(), "folder", ""});
            qDebug() << "folderName: " << info.folderName() << "  url: " << info.url();
        }
//        if(sreq == usmodel) {
/*            qDebug() << "To usModel"*/;
            emit pathsReceived(sl);
        // }
        // else {
        //     // qDebug() << "To imodel";
        //     emit pathsReceived2(sl);
        // }
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
        qDebug() << "fileName" << "response content" << response.content() << "status" << response.status();
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
    pict_data::BucketsRequest message;
    message.setUserLogin("Ivon");

    base.setReqUserBuckets(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
    return {};
}

Q_INVOKABLE int WebSocketClient::deleteFileFromBucketRequest(const QString &filePath){
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
    pict_data::DeleteFileRequest message;
    qDebug() << "deleteFileFromBucketRequest" << filePath << "bucket" << bucket;

    message.setFileName(key);
    message.setMongoId("1111111");
    message.setUserLogin("Ivon");
    message.setBucketName(bucket);

    base.setDeleteFile(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    /*qint64 sz =*/ m_webSocket->sendBinaryMessage(data);
    return m_webSocket->sendBinaryMessage(data);
}

/*Q_INVOKABLE*/ void WebSocketClient::getFilesFoldersListfromBucketRequest(const QString &minioPath){
    qDebug() << "getFilesFoldersListfromBucketRequest(const QString &minioPath)" << minioPath;
    QUrl minioUrl(minioPath);
    QString path = minioUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    int sz = parts.size();
    QString folder = "";
    if(sz > 2) {
        folder = parts.sliced(1, sz - 2).join("/");
    }
    qDebug() << "bucket: " << bucket << " folder " << folder;

    pict_data::BaseMessage base;
    pict_data::FilesFoldersListRequest message;
    message.setFolderName(folder);
    message.setBucketName(bucket);
    message.setUserLogin("Ivon");

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

Q_INVOKABLE int WebSocketClient::addFileRequest(const QString &filePath, const QString &minioPath){
    qDebug() << "addFileRequest(const QStringList &filePath)" << filePath << "minioPath: " << minioPath;
    QString cleanPath = filePath;
    if (cleanPath.startsWith("file:///")) {
        cleanPath = QUrl(cleanPath).toLocalFile();
    }
    QFileInfo fileInfo(cleanPath);

    QString fileName = fileInfo.fileName();

    QUrl minioUrl(minioPath);
    QString path = minioUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    pict_data::BaseMessage base;
    pict_data::AddFileRequest message;

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
    message.setFileName(fileName);
    message.setUserLogin("Ivon");
    message.setBucketName(bucket);
    message.setFolder(folder);
    message.setInfo("jpg");
    message.setData(fileData);

    base.setAddFile(message);
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
    message.setFileName(fileInfo.fileName());
    message.setEmailLogin("forever_young");
    message.setData(fileData);
    message.setContentType("file_1");
    message.setTimestamp(QDateTime::currentMSecsSinceEpoch());

    base.setPict(message);
    QProtobufSerializer serializer;
    QByteArray data = base.serialize(&serializer);
    qint64 sz = m_webSocket->sendBinaryMessage(data);

*/

void WebSocketClient::deleteMinioBucketsRequest(const QStringList &buckets){
    pict_data::BaseMessage base;
    pict_data::DeleteBucketRequest message;

    for(const QString &bucket : buckets) {
        qDebug() << "void WebSocketClient::deleteMinioBucketsRequest(const QStringList &buckets)" << bucket;
        message.setBucketName(bucket);
        base.setDeleteBucket(message);
        QProtobufSerializer serializer;
        QByteArray data = base.serialize(&serializer);
        /*qint64 sz = */ m_webSocket->sendBinaryMessage(data);
    }
}

int WebSocketClient::deleteFileFromServerRequest(const QStringList &fileData){
    if(fileData.size() >= 4) {
        pict_data::BaseMessage base;
        pict_data::DeleteFileRequest message;

        message.setFileName(fileData.at(0));
        message.setBucketName(fileData.at(1));
        message.setMongoId(fileData.at(2));
        message.setUserLogin(fileData.at(3));
        base.setDeleteFile(message);
        QProtobufSerializer serializer;
        QByteArray data = base.serialize(&serializer);
        /*qint64 sz = */ m_webSocket->sendBinaryMessage(data);
    }
    return 0;
}
