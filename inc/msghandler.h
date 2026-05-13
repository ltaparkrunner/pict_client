#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QtProtobuf/QProtobufSerializer>
#include <websocketclient.h>


class MsgHandler : public QObject {
    Q_OBJECT
public:
    explicit MsgHandler(WebSocketClient *client, QObject *parent = nullptr);

    Q_INVOKABLE int getBucketsListRequest() const;
    int addFileRequest(const QString &path, const QString &id, const QString &depart);
    int getFilesFoldersListfromBucketRequest(const QString &path);
    // int getFilesOnlyListfromBucketRequest(const QString &path);

    // Q_INVOKABLE int addFileRequest(const QString &filePath, const QString &path);
    // int deleteMinioBucketsRequest(const QStringList &buckets);
    int deleteFileFromServerRequest(const QStringList &fileData);

signals:
    void startWebSocket(QString token);
    void bucketsReceived(const QStringList &buckets);
    void pathsReceived(const QList<QStringList> &paths);
    void resultSuccess(const QString &msg);
    void resultError(const QString &msg);
public slots:

private slots:
    void handleIncomingServerData(const pict_data::ServerEnvelope &data);


private:
    WebSocketClient *m_client;
    QString token;
};
#endif // MSGHANDLER_H
