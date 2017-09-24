#include "Fireman.h"

#include "LightServer.h"
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>

namespace smart {

Fireman::Fireman(LightServer *server)
{
    mSensorVector.clear();
    m_pServer = server;
    m_pServer->registerCallback(this, dataAvailableCallback);
}

Fireman::~Fireman()
{}


bool Fireman::dataAvailableCallback(void *ptr, RawPackage *pkg)
{
    Fireman *self = (Fireman *)ptr;

    self->parse(self, pkg);

    return false;
}

void Fireman::parse(Fireman *self, RawPackage *pkg)
{
    LOGD("%s: %d bytes -> %s\n", __func__, pkg->length, pkg->payload);
    std::vector<std::string> sp;
    std::string line((const char *)pkg->payload);
    ::split_str(line, sp, ",");

    if(sp.size() < 2) {
        LOGE("%s: failed to parse\n", __func__);
        return;
    }

    FireSensor *data = new FireSensor;
    if(!data) {
        LOGE("%s: failed to allocate firesensor\n", __func__);
        return;
    }
    ::memset(data->id, 0x00, 64);
    ::strcpy(data->id, sp[0].c_str());
    data->ts = ::time(NULL);
    data->size = sp.size() - 1;
    data->val = new float[data->size];
    for(int i = 1; i < sp.size(); i++) {
        data->val[i - 1] = ::atof(sp[i].c_str());
    }

#if 1
    LOGD("timestamp = %ld\n", data->ts);
    LOGD("ID = %s\n", data->id);
    LOGD("size = %d\n", data->size);
    for(int i = 0; i < data->size; i++) {
        LOGD("[%d] -> %f\n", i, data->val[i]);
    }
#endif
}

};
