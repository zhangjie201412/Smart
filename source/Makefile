include config.mk

DIRS=server lightserver modules common tests sensorhub

.PHONY : all clean

all : $(MAKE_ALL)

smartd :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} clean; done
