
include ../../Makefile.inc

PROG=aprof$(EXE)
BINDIR=$(CYCDIR)/bin
CYC_LIB_PATH=$(BINDIR)/lib
SRCS=aprof aprof_event aprof_summary aprof_graph aprof_newgraph aprof_svg
OBJS=$(addsuffix .o, $(SRCS))

install: $(BINDIR)/$(PROG)
$(OBJS): aprof_event.h

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): $(OBJS) $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) aprof aprof.exe $(OBJS)

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -detailedlocation -c $<
