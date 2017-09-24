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
    void hello();

private:
    pthread_t mServerThread;
    std::vector<int> mSockFds;
    std::vector<CallbackData *> mCallbacks;
    int mPort;
    char mServAddr[64];
};
};


#endif
