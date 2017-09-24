#include <stdio.h>
#include <unistd.h>
#include "Fireman.h"
#include "LightServer.h"

using namespace smart;

int main(int argc, char **argv)
{
    LightServer *localServer = new LightServer();

    Fireman *fire = new Fireman(localServer);
    fire->start();

    localServer->setup("127.0.0.1", LOCAL_PORT);
    while(true) {
        ::usleep(1000 * 10000);
    }

    delete fire;

    return 0;
}
