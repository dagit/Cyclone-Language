CC=gcc
CFLAGS = -g -Wall -DNORC -Ddeletes="" 

test: driver.c ../../regions.h ../../regions-norc.o 
	$(CC) $(CFLAGS) driver.c ../../regions-norc.o -o test

../../regions-norc.o: ../../regions.c ../../pages.c ../../alloc.c ../../serialize.c ../../regions.h
	cd ../..; make regions-norc.o REGION_MAIN=1
