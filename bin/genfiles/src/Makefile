
include ../../../Makefile.inc

CFLAGS=-fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -I../../..
#CFLAGS=-O -fwritable-strings -I../../..

PROG=cyclone.exe

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	gcc -O -o $@ $^ ../lib/$(CYCLIB) ../../cyc-lib/gc.a $(LDFLAGS)

clean::
	rm -f $(O_SRCS) $(PROG) *~ 
