

include ../../../Makefile.inc

CFLAGS=-include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -include ../../cyc-lib/include/cyc_include.h
#CFLAGS=-O -fwritable-strings -include ../../cyc-lib/include/cyc_include.h

all: $(CYCLIB)

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar rcs $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) *~
