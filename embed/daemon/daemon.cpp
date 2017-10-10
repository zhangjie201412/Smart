#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "Client.h"

using namespace smart;

int main(int argc, char **argv)
{
    int rc;
    pthread_t thread_id;

    if(argc != 2) {
        LOGE("invalid parameter\n");
        return -1;
    }

    mosqpp::lib_init();
    char *id = argv[1];
    Client *client = new Client(id);
    client->init();

    while(true) {
        rc = client->loop();
        if(rc) {
            client->reconnect();
        }
    }

    mosqpp::lib_cleanup();
    delete client;
}
