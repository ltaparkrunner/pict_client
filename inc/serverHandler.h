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

signals:
    void startWebSocket(QString token);
private slots:
    void handleIncomingServerData(const pict_data::ServerEnvelope &data);

private:
    WebSocketClient *m_client;
    QString token;
};
#endif // SERVERHANDLER_H
