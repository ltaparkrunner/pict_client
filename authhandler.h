#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QtProtobuf/QProtobufSerializer>
#include <websocketclient.h>


class AuthHandler : public QObject {
    Q_OBJECT
public:
    explicit AuthHandler(WebSocketClient *client, QObject *parent = nullptr);
    Q_INVOKABLE void login(const QString &email, const QString &password);
    Q_INVOKABLE void registerUser(const QString &email, const QString &password);

private slots:
    void handleIncomingNetworkData(const QByteArray &data);
private:
    WebSocketClient *m_client;
    QString token;

};

#endif // AUTHHANDLER_H
