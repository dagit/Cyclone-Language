# include ../../Makefile.inc

CYCC = cyclone
CFLAGS = -O3 -DTIMING -Iinclude
CYC_FLAGS = -O3 -DTIMING -nogc -Iinclude

# TO DO: parameterize by language here
EXELANGS := gcc cyclone cyclone-port
JAVALANGS :=  java
LANGS := $(EXELANGS) $(JAVALANGS)

# Not working yet: echo moments
# Not implemented at all: methcall objinst prodcons regexmatch
#  wordfreq
BENCHMARKS := ackermann ary3 except fibo hash hash2 heapsort nestedloop random reversefile sieve sumcol wc lists strcat

N = 11

# hello matrix (after nestedloop), spellcheck (after lists)

EXEFILES = $(foreach lang,$(EXELANGS),$(addprefix $(lang)/, $(addsuffix $(EXE),$(BENCHMARKS))))
JAVAFILES = $(foreach lang,$(JAVALANGS),$(addprefix $(lang)/, $(addsuffix .class,$(BENCHMARKS))))
DIFFFILES = $(addsuffix .diff, $(BENCHMARKS))
CYCHIST=cychist$(EXE)
CATN=catn$(EXE)

all: $(EXEFILES) $(JAVAFILES)

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
	$(RM) $(EXEFILES)
	$(RM) $(JAVAFILES)
	$(RM) $(CYCHIST) $(CATN)
	$(RM) *.diff */*.class *.eps

$(CYCHIST): cychist.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ -lm

%.eps: %.rout
	cat $^ | ./makejgraph | jgraph > $@

%.diff: %.gcc %.cyc
	@(diff $*.gcc $*.cyc > $@; echo $@; exit 0)

%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^ $(LDFLAGS)

java/%.java: java-start/%.java
	./inserttimer < $^ > $@

%.class: %.java
	@(cd `dirname $^`; javac `basename $^`; echo javac $^)

# nobounds-%$(EXE): %.cyc
# 	$(CYCC) --noboundschecks $(CYC_FLAGS) -o $@ $^

nocheck-%$(EXE): %.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
