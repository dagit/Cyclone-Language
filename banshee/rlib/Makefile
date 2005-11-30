CC=gcc
#CFLAGS = -O9 -Wall -Ddeletes=""

ifndef DEBUG
CFLAGS = -O9 -Wall -Ddeletes="" -DNMEMDEBUG -DNDEBUG -fno-common
else
CFLAGS = -Wall -Ddeletes="" -g -pg -fno-common
endif

ifdef REGION_MAIN
CFLAGS := -DREGION_MAIN ${CFLAGS}
endif

TARGETS = libregions.a regions-norc.o

all: $(TARGETS)

test: all
	make clean
	cd test; ./dotest 0 1 2 3 4 5 6 7

time: all
	cd test/2; make timer
	cd test/2; time timer d; time timer b

libregions.a: regions-norc.o
	$(AR) rcs $@ $^

regions-std.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -o regions-std.o regions.c

regions-std-stats.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DREGIONPROFILE -o regions-std-stats.o regions.c

regions-pairs.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DRCPAIRS -o regions-pairs.o regions.c

regions-pairs-stats.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DRCPAIRS -DREGIONPROFILE -o regions-pairs-stats.o regions.c

regions-norc.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DNORC -o regions-norc.o regions.c

clean:
	rm -f *.o $(TARGETS) *~
