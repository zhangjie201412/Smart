include ../config.mk

SERVER_CFLAGS = -I../common/ \
				-I../client/inc/ \
				-I/home/jay/Projects/Smart/libs/target/x86/include
SERVER_LDFLAGS = -lpthread

CFLAGS=-ggdb $(SERVER_CFLAGS)

LDFLAGS= $(SERVER_LDFLAGS)

.PHONY: all clean

Client.o: src/Client.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
