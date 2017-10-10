include ../config.mk

HAL_CFLAGS = -I../hal/ -I../common/
#COMMON_LDFLAGS = -lpthread

CFLAGS=-ggdb $(HAL_CFLAGS)

#LDFLAGS= $(MODULE_LDFLAGS)

.PHONY: all clean

rs485.o: rs485.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o
