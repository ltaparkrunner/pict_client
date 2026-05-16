#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QtProtobuf/QProtobufSerializer>
#include <QSettings>
//  #include <websocketclient.h>


class AuthHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ loggedIn WRITE setLoggedIn NOTIFY loggedInChanged)
public:
    explicit AuthHandler(/*WebSocketClient *client,*/ QObject *parent = nullptr);
//    Q_INVOKABLE void login(const QString &email, const QString &password);
//    Q_INVOKABLE void registerUser(const QString &email, const QString &password);
    Q_INVOKABLE void sendLogin(QString user, QString pass);
    Q_INVOKABLE void sendRegister(QString user, QString pass);
    Q_INVOKABLE void sendAuth(QString user, QString pass, QString path);

    Q_INVOKABLE void logout();
    bool loggedIn() const;
    void setLoggedIn(bool value);
    QString authToken() const { return m_authToken; }
signals:
    void startWebSocket(/*QString token*/);
    void errAuth(QString errmsg);
    void succAuth(QString succmsg);
    void loggedInChanged();
    void loginSuccess();

private slots:
//    void handleIncomingAuthData(const pict_data::AuthResponse &data);
private:
//    WebSocketClient *m_client;
    QSettings settings;
//    QString token;
    QString m_authToken;
    bool m_loggedIn;
};

#endif // AUTHHANDLER_H
