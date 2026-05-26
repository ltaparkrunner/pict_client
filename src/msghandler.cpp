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
        for (const auto &info : response.folders()) {
            sl.append({info.folderName(), info.url(), "folder", ""});
            qDebug() << "folderName: " << info.folderName() << "  url: " << info.url();
        }
        for (const auto &info : response.files()) {
            sl.append({info.fileName(), info.url(), "file", info.mongoId()});
            qDebug() << "fileName: " << info.fileName() << "  url: " << info.url() <<
                "  mongoId: " << info.mongoId() << "  ";
        }
        emit pathsReceived(sl);
    }
    else if(data.contentField() == pict_data::ServerEnvelope::ContentFields::ServerResp) {
        const auto &response = data.serverResp();
        if(response.status() == "success") emit resultSuccess(response.content());
        else emit resultError(response.content());
    }
    else if(data.contentField() == pict_data:: ServerEnvelope::ContentFields::FilesIdsResponse){
        qDebug() << "pict_data:: ServerEnvelope::ContentFields::FilesIdsResponse info.url()";
        const auto &response = data.filesIdsResponse();
        qDebug() << "response: " << response.files()[0].url();
        QVector<QUrl> urls;

        for(const auto &info : response.files()) {
            qDebug() << "info.url" << info.url();
            urls.append(info.url());
        }
        emit writeUrlsToLocal(urls);
    }
    else if(data.contentField() == pict_data:: ServerEnvelope::ContentFields::PathInfResponse){
        qDebug() << "pict_data:: ServerEnvelope::ContentFields::FilesIdsResponse info.url()";
        const auto &response = data.pathInfResponse();
        if(response.result() == "file") {
            qDebug() << "fileTempPath" << response.netStorePath();
            emit pathInfoResp('f',response.netStorePath());
        }
        else if (response.result() == "folder") {
            qDebug() << "folder";
            emit pathInfoResp('d', response.netPath());
        }
        else if (response.result() == "not_exist") {
            qDebug() << "not_exist";
            emit pathInfoResp('n', "");
        }

    }
}

Q_INVOKABLE int MsgHandler::getBucketsListRequest() const{
    qDebug() << "MsgHandler::getBucketsListRequest()";
    pict_data::ClientEnvelope cenv;
    pict_data::BucketsRequest message;

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setReqUserBuckets(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::addFileRequest(const QString &filepath, const QString &id, const QString &netFolderPath, const QString &fname){
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
    qDebug() << "bucket: " << bucket << " folder " << folder << "fname" << fname;


    message.setFileName(fileName);
//    message.setUserLogin("Ivon");
//    message.setBucketName(bucket);
    message.setFolder(fname);
    message.setInfo(completeSuffix);
    message.setData(fileData);

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setAddFile(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::getFilesFoldersListfromBucketRequest(const QString &netPath, const QString &fname){
    qDebug() << "MsgHandler::getFilesFoldersListfromBucketRequest(const QString &minioPath)" << netPath << " fname: " << fname;
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
    qDebug() << "bucket: " << bucket << " folder " << folder << ", fname: " << fname;

    pict_data::ClientEnvelope cenv;
    pict_data::FilesFoldersListRequest message;
    message.setFolderName(fname);
//    message.setBucketName(bucket);
//    message.setUserLogin("Ivon");

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setListRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::getFilesFoldersListfromBucketRequest2(const QString &netPath, const bool isDir){
    qDebug() << "int getFilesFoldersListfromBucketRequest2(const QString &path, const bool isDir)" << netPath;
    QUrl netUrl(netPath);
    QString path = netUrl.path(); // Вернет "/photos/holiday/sun.jpg"
    if (path.startsWith('/')) {
        path.remove(0, 1);
    }

    QStringList parts = path.split('/');
    QString bucket = parts.takeFirst();
    //  qsizetype bucketIdx = path.indexOf(bucket);
    QString folder = "";
    qDebug() << "parts.length(): " << parts.length() << parts[2] << " / "; // << parts[3];
    if(parts.length()>3)   folder = parts.mid(2).join('/');

    // if (bucketIdx != -1) {
    //     qsizetype startPos = bucketIdx + bucket.length();
    //     qsizetype endPos = path.lastIndexOf('/');
    //     if (endPos > startPos) {
    //         if (path.at(startPos) == '/') {
    //             startPos++;
    //         }
    //         qsizetype length = endPos - startPos;
    //         folder = path.sliced(startPos, length);
    //     }
    // }

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

int MsgHandler::getFilesRequest(const QStringList &paths, const QStringList &ids, const QString &arrival){
    pict_data::ClientEnvelope cenv;
    pict_data::FilesIds message;
    message.setMongoIds(ids);
    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setFilesIdsRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz = */ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::getPathInfo(const QString &netPath){
    pict_data::ClientEnvelope cenv;
    pict_data::PathInfoRequest message;

    message.setNetPath(netPath);
    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setPathInfRequest(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz = */ m_client->sendBinaryMessage(data);
    return 0;
}
