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

typedef struct {
    int id;
    int length;
    uint8_t payload[0];
} RawPackage;

void split_str(std::string& s, std::vector<std::string>& v, const std::string& c);

#endif
