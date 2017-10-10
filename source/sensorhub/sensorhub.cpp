#include <stdio.h>
#include <unistd.h>
#include "Fireman.h"
#include "LightServer.h"
#include "Server.h"

using namespace smart;

int main(int argc, char **argv)
{
    int rc;
    LightServer *localServer = new LightServer();

    mosqpp::lib_init();

    Server *server = new Server("sensorhub");
    server->init(localServer);

    Fireman *fire = new Fireman(localServer);
    fire->start();

    localServer->setup("127.0.0.1", LOCAL_PORT);
    while(true) {
        rc = server->loop();
        if(rc) {
            server->reconnect();
        }
    }

    mosqpp::lib_cleanup();

    delete fire;

    return 0;
}
