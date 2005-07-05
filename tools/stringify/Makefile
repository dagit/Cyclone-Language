include ../../Makefile.inc
PROG = stringify$(EXE)
BINDIR = $(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

# The -B tells the compiler to use the right gc.a and runtime_cyc.o
# The -L tells the compiler to use the right libcyc.a

$(PROG): stringify.cyc $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ $< $(LDFLAGS) -nogc

clean:
	$(RM) stringify stringify.exe
