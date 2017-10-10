include ../config.mk

SERVER_CFLAGS = -I../common/ \
				-I../server/inc/ \
				-I../lightserver/inc/ \
				-I/home/jay/Projects/Smart/libs/target/$(TARGET)/include
SERVER_LDFLAGS = -lpthread

CFLAGS=-ggdb $(SERVER_CFLAGS)

LDFLAGS= $(SERVER_LDFLAGS)

.PHONY: all clean

Server.o: src/Server.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
