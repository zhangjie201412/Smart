#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <vector>
#include <string>

#ifdef USE_SYSLOG
#include <syslog.h>
#define LOGE(format, ...)       syslog(LOG_ERR, format, ## __VA_ARGS__)
#define LOGI(format, ...)       syslog(LOG_INFO, format, ## __VA_ARGS__)
#define LOGD(format, ...)       syslog(LOG_DEBUG, format, ## __VA_ARGS__)
#else
#include <stdio.h>
#define LOGE(format, ...)       printf(format, ## __VA_ARGS__)
#define LOGI(format, ...)       printf(format, ## __VA_ARGS__)
#define LOGD(format, ...)       printf(format, ## __VA_ARGS__)
#endif

#define LOCAL_IP                "172.16.108.138"

#define LOCAL_PORT              8989
#define FIREMAN_PORT            8901

#define KEY_DEVICE              "device"
#define KEY_CONTENT             "content"
#define KEY_DATA_TYPE           "type"
#define KEY_NAME                "name"
#define KEY_TYPE                "type"
#define KEY_UNIT                "unit"
#define KEY_VALUE               "value"
#define KEY_SIZE                "size"
#define KEY_TIMESTAMP           "time"

#define TYPE_REALTIME           "R"

#define SENSOR_DATA_TYPE_INT        "int"
#define SENSOR_DATA_TYPE_FLOAT      "float"
#define SENSOR_DATA_TYPE_FLOAT_ARR  "float[]"

#define FIREMAN_SENSOR_NAME     "fireman"
#define FIREMAN_SENSOR_UNIT     "KPa"


typedef struct {
    int id;
    int length;
    uint8_t payload[0];
} RawPackage;

void split_str(std::string& s, std::vector<std::string>& v, const std::string& c);

#endif
