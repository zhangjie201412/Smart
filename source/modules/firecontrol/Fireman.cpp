#include "Fireman.h"

#include "LightServer.h"
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <json/json.h>

namespace smart {

Fireman::Fireman(LightServer *server)
{
    mSensorVector.clear();
    m_pServer = new LightServer();
    m_pLocalServer = server;
}

Fireman::~Fireman()
{
    mSensorVector.clear();
    ::pthread_mutex_destroy(&mMutex);
    ::pthread_cond_destroy(&mCond);

    delete m_pServer;
}

bool Fireman::start()
{
    m_pServer->registerCallback(this, dataAvailableCallback);
    int ret = ::pthread_create(&mOutThread, NULL, Fireman::outThread, this);
    if(ret < 0) {
        LOGE("%s: failed to create out thread\n", __func__);
        return false;
    }
    m_pServer->setup(LOCAL_IP, FIREMAN_PORT);
}

void* Fireman::outThread(void *ptr)
{
    Fireman *self = (Fireman *)ptr;
    while(true) {
        ::pthread_mutex_lock(&self->mMutex);
        //wait
        if(self->mSensorVector.size() == 0) {
            ::pthread_cond_wait(&self->mCond, &self->mMutex);
        }
        for(int i = 0; i < self->mSensorVector.size(); i++) {
            FireSensor *data = self->mSensorVector[i];
            self->makeOutString(data);
            self->mSensorVector.erase(self->mSensorVector.begin() + i);
            delete data;
#if 0
            LOGD("timestamp = %ld\n", data->ts);
            LOGD("ID = %s\n", data->id);
            LOGD("size = %d\n", data->size);
            for(int j = 0; j < data->size; j++) {
                LOGD("[%d] -> %f\n", j, data->val[j]);
            }
#endif
        }
        ::pthread_mutex_unlock(&self->mMutex);
    }

    return NULL;
}

bool Fireman::dataAvailableCallback(void *ptr, RawPackage *pkg)
{
    Fireman *self = (Fireman *)ptr;

    ::pthread_mutex_lock(&self->mMutex);
    self->parse(self, pkg);
    ::pthread_mutex_unlock(&self->mMutex);

    return false;
}

void Fireman::makeOutString(FireSensor *data)
{
    Json::Value root;
    Json::Value contents;
    Json::Value value;
    Json::Value valArr;

    root[KEY_DEVICE] = data->id;
    root[KEY_DATA_TYPE] = TYPE_REALTIME;

    for(int i = 0; i < data->size; i++) {
        valArr[i] = (double)data->val[i];
    }

    value[KEY_NAME] = FIREMAN_SENSOR_NAME;
    value[KEY_TYPE] = SENSOR_DATA_TYPE_FLOAT_ARR;
    value[KEY_SIZE] = data->size;
    value[KEY_VALUE] = valArr;
    value[KEY_UNIT] = FIREMAN_SENSOR_UNIT;
    value[KEY_TIMESTAMP] = (int)data->ts;

    contents.append(value);
    root[KEY_CONTENT] = contents;
    m_pLocalServer->sendToLocalClient((char *)root.toStyledString().c_str(),
            root.toStyledString().size());
}

void Fireman::parse(Fireman *self, RawPackage *pkg)
{
    //LOGD("%s: %d bytes -> %s\n", __func__, pkg->length, pkg->payload);
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
    mSensorVector.push_back(data);
    ::pthread_cond_signal(&self->mCond);

#if 0
    LOGD("timestamp = %ld\n", data->ts);
    LOGD("ID = %s\n", data->id);
    LOGD("size = %d\n", data->size);
    for(int i = 0; i < data->size; i++) {
        LOGD("[%d] -> %f\n", i, data->val[i]);
    }
#endif
}

};
