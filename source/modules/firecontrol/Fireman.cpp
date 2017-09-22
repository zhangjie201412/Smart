#include "Fireman.h"

#include "LightServer.h"

namespace smart {

Fireman::Fireman(LightServer *server)
{
    m_pServer = server;
    m_pServer->registerCallback(this, dataAvailableCallback);
}

Fireman::~Fireman()
{}


bool Fireman::dataAvailableCallback(void *ptr, RawPackage *pkg)
{
    Fireman *self = (Fireman *)ptr;

    self->parse(pkg);

    return false;
}

void Fireman::parse(RawPackage *pkg)
{
    LOGD("%s: %d bytes -> %s\n", __func__, pkg->length, pkg->payload);
}

};
