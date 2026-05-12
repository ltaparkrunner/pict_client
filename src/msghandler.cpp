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
