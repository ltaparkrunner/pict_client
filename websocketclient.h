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
    explicit WebSocketClient(QObject *parent = nullptr);
    Q_INVOKABLE void connectToServer(const QString &url);
    QString lastReceivedPath() const;

signals:
    void pathReceived(QString path);

private slots:
    void onBinaryMessage(const QByteArray &message);

private:
    QWebSocket m_webSocket;
    QString m_path;
};

#endif // WEBSOCKETCLIENT_H
