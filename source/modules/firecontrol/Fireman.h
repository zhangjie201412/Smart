#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#include "common.h"
#include <time.h>
#include <pthread.h>
#include "LightServer.h"

namespace smart {

class LightServer;
class vector;
class Fireman {

typedef struct {
    time_t ts;
    int size;
    char id[64];
    float *val;
} FireSensor;

public:
    Fireman(LightServer *localServer);
    ~Fireman();
    bool start();

private:
    static bool dataAvailableCallback(void *ptr, RawPackage *pkg);
    static void *outThread(void *ptr);
    void parse(Fireman *self, RawPackage *pkg);
    void makeOutString(FireSensor *data);

    LightServer *m_pServer;
    LightServer *m_pLocalServer;
    std::vector<FireSensor *> mSensorVector;
    pthread_t mOutThread;
    pthread_mutex_t mMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mCond = PTHREAD_COND_INITIALIZER;
};
};

#endif
