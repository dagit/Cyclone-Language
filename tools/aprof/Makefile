
include ../../Makefile.inc

PROG=aprof$(EXE)
BINDIR=$(CYCDIR)/bin
CYC_LIB_PATH=$(BINDIR)/lib

install: $(BINDIR)/$(PROG)

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): aprof.o $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ aprof.o $(LDFLAGS)

clean:
	$(RM) aprof aprof.exe *.o

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $<
