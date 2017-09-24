#include <unistd.h>
#include "common.h"
#include "LightServer.h"
#include "Fireman.h"

using namespace smart;

int main(void)
{
    LightServer *server = new LightServer();
    Fireman *fire = new Fireman(server);
    server->setup("127.0.0.1", 8989);

    while(true) {
        ::usleep(1000 * 10000);
    }

    delete fire;
    delete server;

    return 0;
}
