include ../config.mk

TOOL_CFLAGS = -I../common/ -I../hal/
TOOL_LDFLAGS = -lpthread

CFLAGS=-ggdb $(TOOL_CFLAGS)

LDFLAGS= $(TOOL_LDFLAGS)

.PHONY: all clean

all : rs485_utils

rs485_utils : rs485_utils.o ../hal/rs485.o ../common/common.o
	${CXX} $^ -o $@ $(LDFLAGS)

daemon.o: rs485_utils.cpp
	$(CXX) -c $^ -o $@ $(CFLAGS)

clean :
	-rm -f *.o rs485_utils
