
include ../../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../../bin/$(CYCCOMP)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

override CYC_FLAGS += -I../../lib -B../../lib

LIB=../../bin/cyc-lib/$(OUT_PREFIX)$(CYCLIB)

all: $(CYCC) $(OUT_PREFIX)freq$(EXE) $(OUT_PREFIX)strings$(EXE) \
  $(OUT_PREFIX)grep$(EXE) $(OUT_PREFIX)markov$(EXE)


# Note: we leave in $(LIB) so we get the $(OUT_PREFIX) version of the
# library
$(OUT_PREFIX)%$(EXE): $(OUT_PREFIX)%.o $(LIB)
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^ $(LDFLAGS)

$(OUT_PREFIX)freq$(EXE): $(OUT_PREFIX)freq.o $(LIB)
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c -o $@ $<

clean_prefix:
	$(RM) $(addprefix $(OUT_PREFIX), freq.o strings.o markov.o grep.o  *.exe)

clean:
	$(RM) *freq.o *freq.c *strings.o *strings.c *markov.o *markov.c  *grep.o *grep.c 
	$(RM) *~ *.exe *.output *.stackdump
	$(RM) freq strings grep markov
	$(RM) freq array array2 grep strings
