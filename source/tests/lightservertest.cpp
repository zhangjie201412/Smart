#include "LightServer.h"

#include <unistd.h>
#include "common.h"

using namespace smart;

bool data_callback(void *ptr, RawPackage *pkg)
{
    LOGD("%s: %d bytes -> %s\n", (char *)ptr, pkg->length, pkg->payload);
}

int main(void)
{
    char name[4] = "ID";
    LightServer *server = new LightServer();
    server->registerCallback((void *)name, data_callback);
    server->setup("127.0.0.1", 8989);

    while(true) {
        ::usleep(1000 * 10000);
    }

    delete server;

    return 0;
}
