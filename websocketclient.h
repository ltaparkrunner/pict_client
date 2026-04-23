#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QFile>

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastReceivedPath READ lastReceivedPath NOTIFY pathReceived)
public:
    explicit WebSocketClient(const QUrl &url, QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(/*const QString &url*/);
    QString lastReceivedPath() const;
    Q_INVOKABLE QStringList getBucketsList() const;
    /*Q_INVOKABLE*/ void getImagesListfromBucketRequest(const QString &bucket) const;

signals:
    void pathReceived(const QString &path);
    void pathsReceived(const QStringList &paths);

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

    const int RECONNECT_INTERVAL = 5000;
    const int PING_INTERVAL = 30000;
};

#endif // WEBSOCKETCLIENT_H
