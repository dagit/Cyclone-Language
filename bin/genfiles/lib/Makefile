
include ../../../Makefile.inc

CFLAGS=-O2 -fwritable-strings -I../../..

all: $(CYCLIB)

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar qc $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) *~
