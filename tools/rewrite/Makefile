include ../../Makefile.inc

PROG=rewrite$(EXE)
BINDIR=$(CYCDIR)/bin
CYC_LIB_PATH=$(BINDIR)/lib


LC_FLAG =
# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP)

SRCS=rewrite

O_SRCS=$(addsuffix .o, $(SRCS))

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): $(O_SRCS) $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ $(O_SRCS) $(LDFLAGS)

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $< 

clean:
	$(RM) *.o *.c *.output *.stackdump
	$(RM) $(PROG) 

