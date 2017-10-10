include config.mk

DIRS=client common hal daemon tool

.PHONY : all clean

all : $(MAKE_ALL)

smartd :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d} clean; done
