# include ../../Makefile.inc

CYCC = cyclone
CFLAGS = -O3 -DTIMING -Iinclude
CYC_FLAGS = -O3 -DTIMING -Iinclude
# TO DO: only add this library for those execs that need it
LDFLAGS = -lm

EXELANGS := gcc cyclone cyclone-port
JAVALANGS :=  java
LANGS := $(EXELANGS) $(JAVALANGS)

# Skipped: hello
# Obsoleted: ary2
# Not implemented at all: methcall objinst prodcons regexmatch  wordfreq
BENCHMARKS := ackermann ary3 echo except fibo hash hash2 heapsort moments nestedloop random reversefile sieve sumcol wc lists spellcheck strcat matrix
SOMEBENCHMARKS := ackermann ary3 echo fibo hash hash2 heapsort moments nestedloop random reversefile sieve sumcol wc spellcheck strcat matrix

N = 11


EXEFILES = $(foreach lang,$(EXELANGS),$(addprefix $(lang)/, $(addsuffix $(EXE),$(BENCHMARKS))))
JAVAFILES = $(foreach lang,$(JAVALANGS),$(addprefix $(lang)/, $(addsuffix .class,$(BENCHMARKS))))
NOCHECKFILES = $(foreach lang,cyclone cyclone-port,$(addprefix nocheck-$(lang)/, $(addsuffix $(EXE),$(BENCHMARKS))))
DIFFFILES = $(addsuffix .diff, $(BENCHMARKS))
CYCHIST=cychist$(EXE)
CATN=catn$(EXE)

all: nocheck-cyclone nocheck-cyclone-port $(EXEFILES) $(JAVAFILES) $(NOCHECKFILES)

nocheck-cyclone:
	mkdir nocheck-cyclone

nocheck-cyclone-port:
	mkdir nocheck-cyclone-port

run: all $(CATN)
	@(RUN=`date +"%d%b%y-%H%M"`; \
	LANGS="$(LANGS)" BENCHMARKS="$(BENCHMARKS)" ./runtests $(N) > run-$$RUN.rout)

bench: all $(CYCHIST) $(CATN)
	LANGS="$(LANGS)" BENCHMARKS="$(BENCHMARKS)" ./runtests $(N) | ./cychist

diff: $(DIFFFILES)
	@(for file in $(DIFFFILES); do \
	  echo ==== $$file ====; \
	  cat $$file; \
	done)

count:
	wc -l $(addsuffix .gcc, $(BENCHMARKS))

clean:
	$(RM) $(EXEFILES) $(NOCHECKFILES) $(JAVAFILES)
	$(RM) $(CYCHIST) $(CATN)
	$(RM) *.diff */*.class *.eps

$(CYCHIST): cychist.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ -lm

%.eps: %.rout makejgraph
	cat $^ | ./makejgraph | jgraph > $@

%.diff: %.gcc %.cyc
	@(diff $*.gcc $*.cyc > $@; echo $@; exit 0)

%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^ $(LDFLAGS)

java/%.java: java-start/%.java
	./inserttimer < $^ > $@

%.class: %.java
	@(cd `dirname $^`; javac `basename $^`; echo javac $^)

# only the optimized versions get the benefit of no GC
cyclone/%$(EXE): cyclone/%.cyc
	$(CYCC) $(CYC_FLAGS) -nogc -o $@ $^ $(LDFLAGS)

nocheck-cyclone/%$(EXE): cyclone/%.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -nogc -o $@ $^ $(LDFLAGS)

nocheck-cyclone-port/%$(EXE): cyclone-port/%.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
