include ../../Makefile.inc

CYCC = $(CYCDIR)/bin/$(CYCCOMP)
CFLAGS = -O3 -DTIMING
CYC_FLAGS = -O3 -DTIMING

# Not working yet: echo moments
# Not implemented at all: lists methcall objinst prodcons regexmatch
#  spellcheck strcat wordfreq
#BENCHMARKS = ackermann ary3 except fibo hash hash2 heapsort\
# hello matrix moments nestedloop random reversefile sieve sumcol wc
BENCHMARKS = ackermann ary3 except fibo hash hash2 heapsort\
hello matrix nestedloop random reversefile sieve sumcol wc lists \
spellcheck strcat

EXEFILES = $(addsuffix $(EXE), $(BENCHMARKS))
GCCEXEFILES = $(addprefix gcc-, $(EXEFILES))
NOCHECKEXEFILES = $(addprefix nocheck-, $(EXEFILES))
# NOBOUNDSEXEFILES = $(addprefix nobounds-, $(EXEFILES))
HIST=hist$(EXE)
CATN=catn$(EXE)

all: $(EXEFILES) $(GCCEXEFILES) $(NOCHECKEXEFILES)

bench: all $(HIST) $(CATN)
	@(./runtests 11 | ./hist)

diff:
	@(for bm in $(BENCHMARKS); do \
	  echo ==== $$bm ====; diff $$bm.gcc $$bm.cyc; \
	done)

clean:
	$(RM) $(NOCHECKEXEFILES)
	$(RM) $(NOBOUNDSEXEFILES)
	$(RM) $(EXEFILES)
	$(RM) $(GCCEXEFILES)

$(HIST): hist.c
	gcc $(CFLAGS) -o $@ -x c $^

gcc-%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^

# nobounds-%$(EXE): %.cyc
# 	$(CYCC) --noboundschecks $(CYC_FLAGS) -o $@ $^

nocheck-%$(EXE): %.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^
