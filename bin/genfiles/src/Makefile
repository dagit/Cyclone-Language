
include ../../../Makefile.inc

CFLAGS=-fwritable-strings -I../../..
#CFLAGS=-g -fwritable-strings -I../../..
#CFLAGS=-O -fwritable-strings -I../../..

PROG=cyclone.exe

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	gcc -O -o $@ $^ ../lib/$(CYCLIB) ../../gc.a $(LDFLAGS)

clean::
	rm -f $(O_SRCS) $(PROG) *~ 
