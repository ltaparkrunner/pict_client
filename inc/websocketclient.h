#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QFile>
#include <QAuthenticator>
#include "pict_data/message.qpb.h"
#include "authhandler.h"
//#include <QSettings>

// enum sourceReq{
//     imodel,
//     usmodel
// };

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastReceivedPath READ lastReceivedPath NOTIFY pathReceived)
public:
    explicit WebSocketClient(AuthHandler *authHandler, const QUrl &url, QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(/*const QString &url*/);
    QString lastReceivedPath() const;
//    Q_INVOKABLE QStringList getBucketsListRequest() const;
    /*Q_INVOKABLE*/ void getFilesFoldersListfromBucketRequest(const QString &path);
    int getFilesOnlyListfromBucketRequest(const QString &path);
    Q_INVOKABLE int deleteFileFromBucketRequest(const QString &filePath);
//    void getFilesListfromBucketRequest2(const QString &bucket) const;
//    Q_INVOKABLE int addFileRequest(const QStringList &filePath);
    Q_INVOKABLE int addFileRequest(const QString &filePath, const QString &path);
    void deleteMinioBucketsRequest(const QStringList &buckets);
    int deleteFileFromServerRequest(const QStringList &fileData);

    void sendBinaryMessage(const QByteArray &data);
    void wsTokenConnect(QString token);
    void wsConnect();

signals:
    void pathReceived(const QString &path);
    void pathsReceived(const QList<QStringList> &paths);
    void pathsReceived2(const QList<QStringList> &paths);
    void bucketsReceived(const QStringList &buckets);
    void filesReceived(const QList<QStringList> &paths);

    void serverResponseReceived(const pict_data::ServerEnvelope &response);
    void authResponseReceived(const pict_data::AuthResponse &response);
    void authResponseReceived2(const QByteArray &response);
//    void authResponseReceived(const pict_data::AuthResponse &response);
//    void serverMessageReceived(const pict_data::ServerEnvelope &message);
    void showLoginRequired();

public slots:
    void connectToServer2();
    void disconnectFromServer();
private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);

    void onBinaryMessageReceived2(const QByteArray &rawBytes);
    void onAuthRequired(QAuthenticator *authenticator);
public:
    QWebSocket *m_webSocket;
private:
    AuthHandler *m_authHandler;
    QUrl m_url;
//    QTimer m_reconnectTimer;
    QTimer m_pingTimer;
    QString m_path;
//    QSettings settings;
 //   QString token;
//    sourceReq sreq; // What is it? What is for?
    // it defines who is the source of request, and where we have to return the answer?
    // to imodel(ImageModel) or to usmodel(unifiedstoragemodel)
    // pathReceived2 oasses signal to ImageModel(listStringModel)
    // pathsReceived passes signal to isModel(unifiedstoragemodel)

    const int RECONNECT_INTERVAL = 5000;
    const int PING_INTERVAL = 30000;
};

#endif // WEBSOCKETCLIENT_H
