#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <mosquittopp.h>

namespace smart {
class Client: public mosqpp::mosquittopp {
public:
    Client(const char *id);
    virtual ~Client();
    int init();

    void on_connect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);

    int send(char *buf, int size);

private:
    bool mIsConnected;
};
};

#endif
