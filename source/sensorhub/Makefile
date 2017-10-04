include ../config.mk

HUB_CFLAGS = -I../common/ -I../server/inc/ -I../lightserver/inc/ \
			 -I../modules/firecontrol/ \
			 -I/home/jay/Projects/Smart/libs/target/$(TARGET)/include
HUB_LDFLAGS = -lpthread -ljsoncpp -L/home/jay/Projects/Smart/libs/target/$(TARGET)/lib -lmosquittopp -lmosquitto

CFLAGS=-ggdb $(HUB_CFLAGS)

LDFLAGS= $(HUB_LDFLAGS)

.PHONY: all clean

all : sensorhub

sensorhub : sensorhub.o ../modules/Fireman.o ../server/Server.o ../lightserver/LightServer.o ../common/common.o
	${CXX} $^ -o $@ $(LDFLAGS)

sensorhub.o: sensorhub.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o sensorhub
