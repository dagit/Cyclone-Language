
CYCDIR = ../..

include $(CYCDIR)/Makefile.inc

CYCC = $(CYCDIR)/bin/cyclone$(EXE)

CYCCFLAGS = -I$(CYCDIR)/include -B$(CYCDIR)/bin/cyc-lib

aprof$(EXE): aprof.o
	$(CYCC) $(CYCCFLAGS) -o $@ $<

install: aprof$(EXE) $(CYCC)
	cp $< $(CYCDIR)/bin

clean:
	$(RM) aprof$(EXE) *.o

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCCFLAGS) -c $<
