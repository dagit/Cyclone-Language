
# Note: We're not doing anything with t.cyc t2.cyc t3.cyc t4.cyc 
#       testcases.cyc and toplev.cyc.  Should we be?

include ../../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../../bin/$(CYCCOMP)

CYCBISON=../../bin/cycbison.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CFLAGS=-g -fwritable-strings -I../.. 

CYC_FLAGS=-I ../../lib -tc -toc -pp

LIB=../../bin/$(OUT_PREFIX)$(CYCLIB)

all: $(OUT_PREFIX)freq $(OUT_PREFIX)strings $(OUT_PREFIX)grep


$(OUT_PREFIX)%: $(OUT_PREFIX)%.o $(LIB)
	gcc -g -o $@ $^ ../../bin/gc.a $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $< 

clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), freq.o strings.o markov.o grep.o  *.exe)

clean:
	rm -f *freq.o *freq.c *strings.o *strings.c *markov.o *markov.c  *grep.o *grep.c 
	rm -f *~ *.exe *.output *.stackdump
	rm freq array array2 grep strings
