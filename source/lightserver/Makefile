include ../config.mk

LIGHT_CFLAGS = -I../common/ -I../lightserver/inc/
LIGHT_LDFLAGS = -lpthread

CFLAGS=-ggdb $(LIGHT_CFLAGS)

LDFLAGS= $(LIGHT_LDFLAGS)

.PHONY: all clean

LightServer.o: src/LightServer.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
