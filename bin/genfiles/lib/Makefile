

include ../../../Makefile.inc

CFLAGS=-include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-O -fwritable-strings -include ../../cyc-lib/include/cyc_include.h

all: $(CYCLIB) nogc.a

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar rcs $@ $^

nogc.a: nogc.o
	ar rcs $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) nogc.o nogc.a *~
