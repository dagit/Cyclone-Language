
include ../../../Makefile.inc

CFLAGS=-g -fwritable-strings -I../../..

PROG=cyclone.exe

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	gcc -g -o $@ $^ ../lib/$(CYCLIB) ../../gc.a

clean::
	rm -f $(O_SRCS) *~