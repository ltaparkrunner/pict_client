#include "msghandler.h"
#include <QFileInfo>

MsgHandler::MsgHandler(WebSocketClient *client, QObject *parent)
    : QObject(parent), m_client(client)
{
    connect(m_client, &WebSocketClient::serverResponseReceived,
            this, &MsgHandler::handleIncomingServerData);
}

void MsgHandler::handleIncomingServerData(const pict_data::ServerEnvelope &data){
    qDebug() << "MsgHandler::handleIncomingServerData";
    if(data.contentField() == pict_data::ServerEnvelope::ContentFields::Buckets){
        const auto &response = data.buckets();
        QStringList sl;
        for (const auto &info : response.bucketInf()) {
            sl.append(info.bucketName());
            qDebug() << "info.bucketName()" << info.bucketName();
            sl.append(info.url());
            qDebug() << "info.url()" << info.url();
        }
        emit bucketsReceived(sl);
    }
    else if(data.contentField() == pict_data::ServerEnvelope::ContentFields::ListResponse) {
        const auto &response = data.listResponse();
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
        emit pathsReceived(sl);
    }
    else if(data.contentField() == pict_data::ServerEnvelope::ContentFields::ServerResp) {
        const auto &response = data.serverResp();
        if(response.status() == "success") emit resultSuccess(response.content());
        else emit resultError(response.content());
    }
}

Q_INVOKABLE int MsgHandler::getBucketsListRequest() const{
    pict_data::ClientEnvelope cenv;
    pict_data::BucketsRequest message;
//    message.setUserLogin("Ivon");

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setReqUserBuckets(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::addFileRequest(const QString &filepath, const QString &id, const QString &netFolderPath){
    qDebug() << "MsgHandler::addFileRequest" << filepath << " " << id << "  " << netFolderPath;
    pict_data::ClientEnvelope cenv;
    pict_data::AddFileRequest message;

    QFile *file = new QFile(filepath);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file:" << filepath;
        delete file;
        return -1; // Ошибка открытия файла
    }
//    else qDebug() << "Open file: " << path;
    QByteArray fileData = file->readAll();

    QFileInfo fileInfo(filepath);
    QString fileName = fileInfo.fileName();
    QString completeSuffix = fileInfo.completeSuffix();

    /*--------------------*/
    QUrl netUrl(netFolderPath);
    QString path = netUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    qsizetype bucketIdx = path.indexOf(bucket);
    QString folder = "";
    if (bucketIdx != -1) {
        qsizetype startPos = bucketIdx + bucket.length();
        qsizetype endPos = path.lastIndexOf('/');
        if (endPos > startPos) {
            if (path.at(startPos) == '/') {
                startPos++;
            }
            qsizetype length = endPos - startPos;
            folder = path.sliced(startPos, length);
        }
    }
    qDebug() << "bucket: " << bucket << " folder " << folder;


    message.setFileName(fileName);
//    message.setUserLogin("Ivon");
//    message.setBucketName(bucket);
    message.setFolder(folder);
    message.setInfo(completeSuffix);
    message.setData(fileData);

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setAddFile(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::getFilesFoldersListfromBucketRequest(const QString &netPath){
    qDebug() << "getFilesFoldersListfromBucketRequest(const QString &minioPath)" << netPath;
    QUrl netUrl(netPath);
    QString path = netUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    qsizetype bucketIdx = path.indexOf(bucket);
    QString folder = "";
    if (bucketIdx != -1) {
        qsizetype startPos = bucketIdx + bucket.length();
        qsizetype endPos = path.lastIndexOf('/');
        if (endPos > startPos) {
            if (path.at(startPos) == '/') {
                startPos++;
            }
            qsizetype length = endPos - startPos;
            folder = path.sliced(startPos, length);
        }
    }
    qDebug() << "bucket: " << bucket << " folder " << folder;

    pict_data::ClientEnvelope cenv;
    pict_data::FilesFoldersListRequest message;
    message.setFolderName(folder);
//    message.setBucketName(bucket);
//    message.setUserLogin("Ivon");

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setListRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::deleteFileFromServerRequest(const QStringList &fileData){
    if(fileData.size() >= 4) {
        pict_data::ClientEnvelope cenv;
        pict_data::DeleteFileRequest message;
        message.setFileName(fileData.at(0));
//        message.setBucketName(fileData.at(1));
        message.setMongoId(fileData.at(2));
//        message.setUserLogin(fileData.at(3));

        cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
        cenv.setDeleteFile(message);
        QProtobufSerializer serializer;
        QByteArray data = cenv.serialize(&serializer);
        /*qint64 sz = */ m_client->sendBinaryMessage(data);
        return 0;
    }
    return -1;
}
