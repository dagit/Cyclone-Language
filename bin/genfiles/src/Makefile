
include ../../../Makefile.inc

CFLAGS=-include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-O -fwritable-strings -include ../../cyc-lib/include/cyc_include.h

PROG=cyclone$(EXE)

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	gcc -O -o $@ $^ ../lib/$(CYCLIB) ../../cyc-lib/gc.a $(LDFLAGS)

clean::
	rm -f $(O_SRCS) $(PROG) *~ 
