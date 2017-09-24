include ../config.mk

HUB_CFLAGS = -I../common/ -I../lightserver/inc/ -I../modules/firecontrol/
HUB_LDFLAGS = -lpthread -ljsoncpp

CFLAGS=-ggdb $(HUB_CFLAGS)

LDFLAGS= $(HUB_LDFLAGS)

.PHONY: all clean

all : sensorhub

sensorhub : sensorhub.o ../modules/Fireman.o ../lightserver/LightServer.o ../common/common.o
	${CXX} $^ -o $@ $(LDFLAGS)

sensorhub.o: sensorhub.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o sensorhub
