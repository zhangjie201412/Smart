# Comment out to disable client client threading support.
WITH_THREADING:=yes
VERSION=1.4.12
TIMESTAMP:=$(shell date "+%F %T%z")

# Client library SO version. Bump if incompatible API/ABI changes are made.
SOVERSION=1
UNAME:=$(shell uname -s)

CFLAGS?=-Wall -ggdb -O2

LIB_CFLAGS:=${CFLAGS} ${CPPFLAGS} -I. -I..
LIB_CXXFLAGS:=$(LIB_CFLAGS) ${CPPFLAGS}
LIB_LDFLAGS:=${LDFLAGS}

LIB_LIBS:=
PASSWD_LIBS:=

#CLIENT_LDFLAGS:=$(LDFLAGS) -L../lib ../lib/libmosquitto.so.${SOVERSION}

LIB_CFLAGS:=$(LIB_CFLAGS) -fPIC
LIB_CXXFLAGS:=$(LIB_CXXFLAGS) -fPIC

TOP_PATH=$(shell pwd)

ifeq ($(WITH_THREADING),yes)
	LIB_LIBS:=$(LIB_LIBS) -lpthread
	LIB_CFLAGS:=$(LIB_CFLAGS) -DWITH_THREADING
endif
CC=/usr/local/arm_linux_4.8/bin/arm-linux-gcc
CXX=/usr/local/arm_linux_4.8/bin/arm-linux-g++
MAKE_ALL:=smartd

INSTALL?=install
