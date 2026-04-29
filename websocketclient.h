#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QFile>

enum sourceReq{
    model,
    usmodel
};

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastReceivedPath READ lastReceivedPath NOTIFY pathReceived)
public:
    explicit WebSocketClient(const QUrl &url, QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(/*const QString &url*/);
    QString lastReceivedPath() const;
    Q_INVOKABLE QStringList getBucketsListRequest() const;
    /*Q_INVOKABLE*/ void getFilesFoldersListfromBucketRequest(const QString &bucket, sourceReq sr);
    Q_INVOKABLE int deleteFileFromBucketRequest(const QString &filePath);
//    void getFilesListfromBucketRequest2(const QString &bucket) const;
//    Q_INVOKABLE int addFileRequest(const QStringList &filePath);
    Q_INVOKABLE int addFileRequest(const QString &filePath, const QString &path);
signals:
    void pathReceived(const QString &path);
    void pathsReceived(const QList<QStringList> &paths);
    void pathsReceived2(const QList<QStringList> &paths);
    void bucketsReceived(const QStringList &buckets);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);

public:
    QWebSocket *m_webSocket;
private:
    QUrl m_url;
    QTimer m_reconnectTimer;
    QTimer m_pingTimer;
    QString m_path;
    sourceReq sreq;

    const int RECONNECT_INTERVAL = 5000;
    const int PING_INTERVAL = 30000;
};

#endif // WEBSOCKETCLIENT_H
