#include "Server.h"
#include "LightServer.h"
#include "common.h"

namespace smart {

#define SERVER_PORT             1883
#define SERVER_KEEPALIVE        60

Server::Server(const char *id)
    : mosquittopp(id)
{}

Server::~Server()
{}

int Server::init(LightServer *light)
{
    int rc = 0;

    m_pLocalServer = light;
    LOGD("connect %s:%d\n", LOCAL_IP, SERVER_PORT);
    connect(LOCAL_IP, SERVER_PORT, SERVER_KEEPALIVE);

    return rc;
}

void Server::on_connect(int rc)
{
    LOGD("%s: connected with code %d\n", __func__, rc);
    if(rc == 0) {
        subscribe(NULL, "sensorhub/main");
    }
}

void Server::on_message(const struct mosquitto_message *message)
{
    LOGD("topic:   %s\n", message->topic);
    LOGD("payload: %s\n", (char *)message->payload);
    //if(m_pLocalServer != NULL) {
    //    m_pLocalServer->sendToLocalClient((char *)message->payload, message->payloadlen);
    //}
}

void Server::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    LOGD("%s: subscription succeed.\n", __func__);
}

}
