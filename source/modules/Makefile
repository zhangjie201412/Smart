include ../config.mk

MODULE_CFLAGS = -I../common/ -I../lightserver/inc/ -Ifirecontrol/
MODULE_LDFLAGS = -lpthread

CFLAGS=-ggdb $(MODULE_CFLAGS)

LDFLAGS= $(MODULE_LDFLAGS)

.PHONY: all clean

Fireman.o: firecontrol/Fireman.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
