#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#include "common.h"
#include <time.h>

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
    Fireman(LightServer *server);
    ~Fireman();

private:
    static bool dataAvailableCallback(void *ptr, RawPackage *pkg);
    void parse(Fireman *self, RawPackage *pkg);
    LightServer *m_pServer;
    std::vector<FireSensor *> mSensorVector;
};
};

#endif
