

include ../../../Makefile.inc

CFLAGS=-fwritable-strings -I../../..
#CFLAGS=-g -fwritable-strings -I../../..
#CFLAGS=-O -fwritable-strings -I../../..

all: $(CYCLIB)

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar qc $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) *~
