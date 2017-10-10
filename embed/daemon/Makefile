include ../config.mk

DAEMON_CFLAGS = -I../common/ -I../client/inc/ \
			 	-I/home/jay/Projects/Smart/libs/target/x86/include
DAEMON_LDFLAGS = -lpthread \
				 -L/home/jay/Projects/Smart/libs/target/arm/lib \
				 /home/jay/Projects/Smart/libs/target/arm/lib/libmosquitto.so.1 \
				 /home/jay/Projects/Smart/libs/target/arm/lib/libmosquittopp.so.1 \
				 -lcares \
				 -luuid


CFLAGS=-ggdb $(DAEMON_CFLAGS)

LDFLAGS= $(DAEMON_LDFLAGS)

.PHONY: all clean

all : daemon

daemon : daemon.o ../client/Client.o ../common/common.o
	${CXX} $^ -o $@ $(LDFLAGS)

daemon.o: daemon.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o daemon
