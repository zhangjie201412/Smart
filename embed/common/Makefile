include ../config.mk

COMMON_CFLAGS = -I../common/
#COMMON_LDFLAGS = -lpthread

CFLAGS=-ggdb $(COMMON_CFLAGS)

#LDFLAGS= $(MODULE_LDFLAGS)

.PHONY: all clean

common.o: common.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
