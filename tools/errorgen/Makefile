# Makefile for errorgen

include ../../Makefile.inc
PROG = errorgen$(EXE)
BINDIR = $(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

SRCS = errorgen

O_SRCS = $(addsuffix .o, $(SRCS))

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

# The -B tells the compiler to use the right gc.a and runtime_cyc.o
# The -L tells the compiler to use the right libcyc.a
$(PROG): $(O_SRCS) $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ $(O_SRCS) $(LDFLAGS)

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $<

clean:
	$(RM) *.o *.c core *~ *.stackdump $(PROG)

