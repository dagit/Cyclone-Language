
include ../../../Makefile.inc

CFLAGS=-O -fwritable-strings -I../../..

PROG=cyclone.exe

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	gcc -O -o $@ $^ ../lib/$(CYCLIB) ../../gc.a

clean::
	rm -f $(O_SRCS) $(PROG) *~ 
