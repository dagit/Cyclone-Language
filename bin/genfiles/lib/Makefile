

include ../../../Makefile.inc

CFLAGS=-fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -I../../..
#CFLAGS=-O -fwritable-strings -I../../..

all: $(CYCLIB)

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar qcs $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) *~
