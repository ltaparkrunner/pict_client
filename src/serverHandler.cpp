#include "serverHandler.h"

ServerHandler::ServerHandler(WebSocketClient *client, QObject *parent)
    : QObject(parent), m_client(client)
{
    connect(m_client, &WebSocketClient::serverResponseReceived,
            this, &ServerHandler::handleIncomingServerData);
}

void ServerHandler::handleIncomingServerData(const pict_data::ServerEnvelope &data){

}
