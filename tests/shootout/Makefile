include ../../Makefile.inc

CYCC = $(CYCDIR)/bin/$(CYCCOMP)
CFLAGS = -O3 -DTIMING
CYC_FLAGS = -O3 -DTIMING -nogc

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
CYCEXEFILES = $(addprefix cyc-, $(EXEFILES))
# NOBOUNDSEXEFILES = $(addprefix nobounds-, $(EXEFILES))
DIFFFILES = $(addsuffix .diff, $(BENCHMARKS))
CYCHIST=cychist$(EXE)
CATN=catn$(EXE)

all: $(CYCEXEFILES) $(EXEFILES) $(GCCEXEFILES) $(NOCHECKEXEFILES)

bench: all $(CYCHIST) $(CATN)
	@(./runtests 11 | ./cychist)

diff: $(DIFFFILES)
	@(for file in $(DIFFFILES); do \
	  echo ==== $$file ====; \
	  cat $$file; \
	done)

count:
	wc -l $(addsuffix .gcc, $(BENCHMARKS))

clean:
	$(RM) $(NOCHECKEXEFILES) $(NOBOUNDSEXEFILES) $(CYCEXEFILES)
	$(RM) $(EXEFILES)
	$(RM) $(GCCEXEFILES)
	$(RM) $(CYCHIST) $(CATN)
	$(RM) *.diff

$(CYCHIST): cychist.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ -lm
#gcc $(CFLAGS) -o $@ -x c $^ -lm

%.diff: %.gcc %.cyc
	@(diff $*.gcc $*.cyc > $@; echo $@; exit 0)

gcc-%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^ $(LDFLAGS)

# nobounds-%$(EXE): %.cyc
# 	$(CYCC) --noboundschecks $(CYC_FLAGS) -o $@ $^

nocheck-%$(EXE): %.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

cyc-%$(EXE): cyc-%.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
