include ../../Makefile.inc

PROG=cycflex$(EXE)
BINDIR=$(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib

# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYC_FLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 
CYCLEX:=$(BINDIR)/cyclex

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $^ $@

$(PROG): cycflex.$(O) $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib -o $@ $< $(LDFLAGS)

cycflex.cyc: cycflex.cyl
	$(CYCLEX) $< $@

%.$(O): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

clean:
	$(RM) cycflex.cyc cycflex.o cycflex.obj cycflex.exe cycflex
