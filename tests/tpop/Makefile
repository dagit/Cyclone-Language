
include ../../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../../src/$(CYCCOMP)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CYC_FLAGS=-I../../lib -B../../lib

LIB=../../lib/$(OUT_PREFIX)$(CYCLIB)

all: $(OUT_PREFIX)freq $(OUT_PREFIX)strings $(OUT_PREFIX)grep $(OUT_PREFIX)markov $(CYCC)


# Note: we leave in $(LIB) so we get the $(OUT_PREFIX) version of the
# library
$(OUT_PREFIX)%: $(OUT_PREFIX)%.o $(LIB)
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c -o $@ $<

clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), freq.o strings.o markov.o grep.o  *.exe)

clean:
	rm -f *freq.o *freq.c *strings.o *strings.c *markov.o *markov.c  *grep.o *grep.c 
	rm -f *~ *.exe *.output *.stackdump
	rm -f freq array array2 grep strings
