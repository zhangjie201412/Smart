#include "LightServer.h"

#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include "common.h"

//#define LOCAL_IP        "192.168.3.95"
//#define LOCAL_PORT      8989

#define MAXEVENTS       4096
#define MAX_LISTENER    2048

namespace smart {

LightServer::LightServer()
    :mPort(-1)
{
    mSockFds.clear();
    mCallbacks.clear();
    ::memset(mServAddr, 0x00, 64);
}

LightServer::~LightServer()
{
    for(int i = 0; i < mSockFds.size(); i++) {
        ::close(mSockFds[i]);
    }
    mSockFds.clear();
    mCallbacks.clear();
}

void *LightServer::serverThread(void *ptr)
{
    int sock_id;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    LightServer *self = (LightServer *)ptr;
    const int MAX_SIZE = 1024;
    uint8_t recv[MAX_SIZE];

    int rc;
    sock_id = ::socket(AF_INET, SOCK_STREAM, 0);
    if(sock_id < 0) {
        LOGE("%s: failed to create socket\n", __func__);
        return NULL;
    }

    struct sockaddr_in local_addr;
    ::memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(self->mServAddr);
    local_addr.sin_port = htons(self->mPort);

    rc = ::bind(sock_id, (struct sockaddr *)&local_addr,
            sizeof(struct sockaddr));
    if(rc < 0) {
        LOGE("%s: bind %s:%d failed: %s\n", __func__,
                self->mServAddr, self->mPort, ::strerror(errno));
        ::close(sock_id);
        return NULL;
    }
    if(self->makeNoneBlock(sock_id) < 0) {
        LOGE("%s: failed make none block\n", __func__);
        ::close(sock_id);
        return NULL;
    }
    rc = ::listen(sock_id, MAX_LISTENER);
    if(rc < 0) {
        LOGE("%s: listen %s:%d failed\n", __func__,
                self->mServAddr, self->mPort);
        ::close(sock_id);
        return NULL;
    }

    efd = ::epoll_create(MAX_LISTENER);
    if(efd < 0) {
        LOGE("%s: failed to create epoll: %s\n",
                __func__, ::strerror(errno));
        ::close(sock_id);
        return NULL;
    }

    event.data.fd = sock_id;
    event.events = EPOLLIN | EPOLLET;
    rc = ::epoll_ctl(efd, EPOLL_CTL_ADD, sock_id, &event);
    if(rc < 0) {
        LOGE("%s:%d failed to epoll ctl %d: %s\n",
                __func__, __LINE__, efd, ::strerror(errno));
        ::close(sock_id);
        return NULL;
    }

    events = (struct epoll_event *)::calloc(MAXEVENTS, sizeof(struct epoll_event));
    while(true) {
        int event_num, i;

        event_num = ::epoll_wait(efd, events, MAXEVENTS, -1);
        for(i = 0; i < event_num; i++) {
            if((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                LOGE("%s: connection closed\n", __func__);
                ::close(events[i].data.fd);
                int idx = 0;
                for(int j = 0; j < self->mSockFds.size(); i++) {
                    if(events[i].data.fd == self->mSockFds[i]) {
                        idx = i;
                        break;
                    }
                }
                self->mSockFds.erase(self->mSockFds.begin() + idx);

                continue;
            } else if(sock_id == events[i].data.fd) {
                //connection comming
                struct sockaddr_in remote_addr;
                int sin_size = sizeof(struct sockaddr_in);
                LOGD("%s: wait for connected...\n", __func__);
                int fd = ::accept(sock_id, (struct sockaddr *)&remote_addr,
                        (socklen_t *)&sin_size);
                if(fd < 0) {
                    LOGE("%s: failed to do accept\n", __func__);
                    continue;
                }
                LOGD("%s: connection done\n", __func__);
                if(self->makeNoneBlock(fd) < 0) {
                    LOGE("%s: failed make none block\n", __func__);
                    continue;
                }
                event.data.fd = fd;
                event.events = EPOLLIN | EPOLLET;
                rc = ::epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
                if(rc < 0) {
                    LOGE("%s:%d failed to epoll ctl\n", __func__, __LINE__);
                    continue;
                }
                self->mSockFds.push_back(fd);
                LOGD("%s: add socket fd: %d, total %ld clients\n",
                        __func__, fd, self->mSockFds.size());
            } else {
                //data available on fd
                int fd = events[i].data.fd;
                ::memset(recv, 0x00, MAX_SIZE);
                int bytes = ::read(fd, recv, MAX_SIZE);
                if(bytes < 0) {
                    LOGE("%s: failed to read bytes: %s\n", __func__, ::strerror(errno));
                    continue;
                } else if(bytes == 0) {
                    LOGD("%s: maybe remote client closed\n", __func__);
                    int idx = 0;
                    for(int j = 0; j < self->mSockFds.size(); i++) {
                        if(fd == self->mSockFds[i]) {
                            idx = i;
                            break;
                        }
                    }
                    self->mSockFds.erase(self->mSockFds.begin() + idx);
                    continue;
                }
                RawPackage *pkg = (RawPackage *)::malloc(sizeof(RawPackage) + bytes);
                if(!pkg) {
                    LOGE("%s: failed to allocate package buffer\n", __func__);
                    continue;
                }
                ::memset(pkg->payload, 0x00, bytes);
                pkg->length = bytes;
                ::memcpy(pkg->payload, recv, bytes);
                bool stop = false;
                for(int j = 0; j < self->mCallbacks.size(); j++) {
                    if(self->mCallbacks[j] != NULL) {
                        stop = self->mCallbacks[i]->callback(self->mCallbacks[i]->data, pkg);
                        if(stop) {
                            break;
                        }
                    }
                }

                ::free(pkg);
            }
        }
    }

    return NULL;
}

int LightServer::makeNoneBlock(int fd)
{
    int flags, ret;

    flags = ::fcntl(fd, F_GETFL, 0);
    if(flags == -1) {
        LOGE("%s: failed to fcntl\n", __func__);
        return -1;
    }

    flags |= O_NONBLOCK;
    ret = ::fcntl(fd, F_SETFL, flags);
    if(flags == -1) {
        LOGE("%s: failed to fcntl\n", __func__);
        return -1;
    }

    return 0;
}

bool LightServer::setup(const char *addr, int port)
{
    mPort = port;
    ::strcpy(mServAddr, addr);
    int ret = ::pthread_create(&mServerThread, NULL, LightServer::serverThread, this);
    if(ret < 0) {
        LOGE("%s: failed to create core thread\n", __func__);
        return false;
    }
    return true;
}

void LightServer::registerCallback(void *ptr, RawPackageCallback callback)
{
    CallbackData *data = new CallbackData;
    data->callback = callback;
    data->data = ptr;
    mCallbacks.push_back(data);
}

void LightServer::send(char *buf, int size)
{
    for(int i = 0; i < mSockFds.size(); i++) {
        if(mSockFds[i] > 0) {
            write(mSockFds[i], buf, size);
        }
    }    
}

void LightServer::sendToLocalClient(char *buf, int size)
{
    uint8_t checksum = 0x00;
    char *sendBuf = (char *)::malloc(size + 5);

    SendData *data = new SendData;
    data->head[0] = 0x55;
    data->head[1] = 0x55;
    data->length = size;
    data->buffer = (uint8_t *)::malloc(size);
    ::memcpy(data->buffer, buf, size);

    for(int i = 0; i < size; i++) {
        checksum += buf[i];
    }
    data->checksum = checksum;
    ::memcpy(sendBuf, data, size + 5);
    send(sendBuf, size + 5);
    ::free(data);
    ::free(sendBuf);
}

void LightServer::hello()
{
}

}
