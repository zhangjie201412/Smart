#include "Server.h"
#include "common.h"

namespace smart {

#define SERVER_HOST             "localhost"
#define SERVER_PORT             1883
#define SERVER_KEEPALIVE        60

Server::Server(const char *id)
    : mosquittopp(id)
{}

Server::~Server()
{}

int Server::init()
{
    int rc = 0;

    LOGD("connect %s:%d\n", SERVER_HOST, SERVER_PORT);
    connect(SERVER_HOST, SERVER_PORT, SERVER_KEEPALIVE);

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
}

void Server::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    LOGD("%s: subscription succeed.\n", __func__);
}

}
