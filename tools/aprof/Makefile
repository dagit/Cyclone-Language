
CYCDIR = ../..

include $(CYCDIR)/Makefile.inc

PROG=cyclex$(EXE)
BINDIR=$(CYCDIR)/bin

CYCC = $(CYCDIR)/bin/cyclone$(EXE)

CYC_FLAGS += -I$(CYCDIR)/include -B$(CYCDIR)/bin/cyc-lib

$(PROG): aprof.o
	$(CYCC) $(CYCCFLAGS) -o $@ $< -lm

all: $(PROG)

install: all $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $(PROG) $(BINDIR)

clean:
	$(RM) aprof aprof.exe *.o

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $<
