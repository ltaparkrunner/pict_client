#include "msghandler.h"

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
}

Q_INVOKABLE int MsgHandler::getBucketsListRequest() const{
    pict_data::ClientEnvelope cenv;
    pict_data::BucketsRequest message;
    message.setUserLogin("Ivon");

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setReqUserBuckets(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}

int MsgHandler::addFileRequest(const QString &path, const QString &id, const QString &depart){
    qDebug() << "MsgHandler::addFileRequest" << path << " " << id << "  " << depart;
    pict_data::ClientEnvelope cenv;
    pict_data::AddFileRequest message;

    QFile *file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file:" << path;
        delete file;
        return -1; // Ошибка открытия файла
    }
    else qDebug() << "Open file: " << path;
    QByteArray fileData = file->readAll();
    message.setFileName("forever.jpg");
    message.setUserLogin("Ivon");
    message.setBucketName("images");
    message.setFolder("my_1");
    message.setInfo("jpg");
    message.setData(fileData);

    cenv.setType(pict_data::ClientEnvelope::Type::CLIENT_MESSAGE);
    cenv.setAddFile(message);
    QProtobufSerializer serializer;
    QByteArray data = cenv.serialize(&serializer);
    /*qint64 sz =*/ m_client->sendBinaryMessage(data);
    return 0;
}
