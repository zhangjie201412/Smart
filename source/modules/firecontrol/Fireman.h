#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#include "common.h"

namespace smart {

class LightServer;
class Fireman {
public:
    Fireman(LightServer *server);
    ~Fireman();

private:
    static bool dataAvailableCallback(void *ptr, RawPackage *pkg);
    void parse(RawPackage *pkg);
    LightServer *m_pServer;
};
};

#endif
