include ../config.mk

TEST_CFLAGS = -I../common/ -I../lightserver/inc/ -I../modules/firecontrol/
TEST_LDFLAGS = -lpthread -ljsoncpp

CFLAGS=-ggdb $(TEST_CFLAGS)

LDFLAGS= $(TEST_LDFLAGS)

.PHONY: all clean

all : lightservertest firemantest

lightservertest : lightservertest.o ../lightserver/LightServer.o
	${CXX} $^ -o $@ $(LDFLAGS)

lightservertest.o: lightservertest.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

firemantest : firemantest.o ../modules/Fireman.o ../lightserver/LightServer.o ../common/common.o
	${CXX} $^ -o $@ $(LDFLAGS)

firemantest.o: firemantest.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o lightservertest firemantest
