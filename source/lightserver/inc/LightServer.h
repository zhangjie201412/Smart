#ifndef __LIGHT_SERVER_H__
#define __LIGHT_SERVER_H__

#include <pthread.h>
#include <vector>

#include "common.h"

namespace smart {

class vector;
class LightServer
{
public:
typedef struct {
    char head[2];
    uint16_t length;
    uint8_t *buffer;
    uint8_t checksum;
} SendData __attribute__((aligned(1)));
    typedef bool (*RawPackageCallback)(void *ptr, RawPackage *pkg);

    typedef struct {
        RawPackageCallback callback;
        void *data;
    } CallbackData;

    static void *serverThread(void *ptr);

    LightServer();
    virtual ~LightServer();

    bool setup(const char *addr, int port);
    int makeNoneBlock(int fd);
    void registerCallback(void *ptr, RawPackageCallback callback);
    void sendToLocalClient(char *buf, int size);
    void hello();

private:
    void send(char *buf, int size);

    pthread_t mServerThread;
    std::vector<int> mSockFds;
    std::vector<CallbackData *> mCallbacks;
    int mPort;
    char mServAddr[64];
};
};


#endif
