#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QtProtobuf/QProtobufSerializer>
#include <websocketclient.h>


class ServerHandler : public QObject {
    Q_OBJECT
public:
    explicit ServerHandler(WebSocketClient *client, QObject *parent = nullptr);

    Q_INVOKABLE int getBucketsListRequest() const;
    // int getFilesFoldersListfromBucketRequest(const QString &path);
    // int getFilesOnlyListfromBucketRequest(const QString &path);

    // Q_INVOKABLE int addFileRequest(const QString &filePath, const QString &path);
    // int deleteMinioBucketsRequest(const QStringList &buckets);
    // int deleteFileFromServerRequest(const QStringList &fileData);

signals:
    void startWebSocket(QString token);
    void bucketsReceived(const QStringList &buckets);

private slots:
    void handleIncomingServerData(const pict_data::ServerEnvelope &data);

private:
    WebSocketClient *m_client;
    QString token;
};
#endif // SERVERHANDLER_H
