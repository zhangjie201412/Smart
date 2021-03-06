#ifndef __SERVER_H__
#define __SERVER_H__

#include <mosquittopp.h>

namespace smart {

class LightServer;
class Server: public mosqpp::mosquittopp {

public:
    Server(const char *id);
    virtual ~Server();

    int init(LightServer *light);
    void on_connect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);

private:
    LightServer *m_pLocalServer;
};

};

#endif
