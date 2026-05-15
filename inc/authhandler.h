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
    Q_INVOKABLE void sendLogin(QString user, QString pass);
    Q_INVOKABLE void sendRegister(QString user, QString pass);
    Q_INVOKABLE void sendAuth(QString user, QString pass, QString path);

signals:
    void startWebSocket(QString token);
    void errAuth(QString errmsg);
    void succAuth(QString succmsg);
private slots:
    void handleIncomingAuthData(const pict_data::AuthResponse &data);
private:
    WebSocketClient *m_client;
    QString token;

};

#endif // AUTHHANDLER_H
