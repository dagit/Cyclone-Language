
include ../../Makefile.inc

PROG=aprof$(EXE)
BINDIR=$(CYCDIR)/bin

install: $(BINDIR)/$(PROG)

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool before
# installing the include files and libraries.  Also we use a particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(BINDIR)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): aprof.o $(BINDIR)/cyc-lib/$(CYCLIB)
	$(CYCC) $(CYCFLAGS) -o $@ aprof.o $(LDFLAGS)

clean:
	$(RM) aprof aprof.exe *.o

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $<
