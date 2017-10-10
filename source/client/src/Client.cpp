#include "Client.h"
#include "common.h"

namespace smart {

#define CLIENT_PORT             1883
#define CLIENT_KEEPALIVE        60

#define MAIN_TOPIC              "sensorhub/main"

#define REMOTE_IP               "116.62.186.169"

Client::Client(const char *id)
    : mosquittopp(id)
{}

Client::~Client()
{}

int Client::init()
{
    int rc = 0;

    LOGD("connect %s:%d\n", REMOTE_IP, CLIENT_PORT);
    mIsConnected = false;
    connect(REMOTE_IP, CLIENT_PORT, CLIENT_KEEPALIVE);

    return rc;
}

void Client::on_connect(int rc)
{
    LOGD("%s: connected with code %d\n", __func__, rc);
    if(rc == 0) {
        mIsConnected = true;
    }
}

void Client::on_message(const struct mosquitto_message *message)
{
    LOGD("topic:   %s\n", message->topic);
}

void Client::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    LOGD("%s: subscription succeed.\n", __func__);
}

int Client::send(char *buf, int size)
{
    if(!mIsConnected) {
        LOGD("%s: not connected\n", __func__);
        return -1;
    }
    return publish(NULL, MAIN_TOPIC, size, buf);
}


}
