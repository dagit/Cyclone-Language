include ../../Makefile.inc

DIFF=~/src/cyclone_internal/benchmarks/bin/diff.pl

CYCC = cyclone
CFLAGS = -O3 -DTIMING -Iinclude
CYC_FLAGS = -O3 -DTIMING -Iinclude
# TO DO: only add this library for those execs that need it
LDFLAGS = -lm -lpcre

EXELANGS := gcc cyclone cyclone-port
JAVALANGS :=  java
LANGS := $(EXELANGS) $(JAVALANGS)

# Skipped: hello
# Obsoleted: ary2
# Cannot implement: methcall objinst (can't do width-subtyping)
#   prodcons (no pthreads, by default)
NOGC_BENCHMARKS := ackermann echo except fibo nestedloop random sieve sumcol wc matrix
NOGC_OPT_BENCHMARKS := reversefile ary3
GC_BENCHMARKS := hash hash2 heapsort moments lists spellcheck strcat wordfreq regexmatch
BENCHMARKS := $(NOGC_BENCHMARKS) $(NOGC_OPT_BENCHMARKS) $(GC_BENCHMARKS)
ifeq ($(HAVE_PTHREAD),yes)
BENCHMARKS += prodcons
endif

DIFF_BENCHMARKS=ackermann echo except fibo nestedloop random sieve sumcol wc matrix reversefile ary3 hash hash2 heapsort moments lists spellcheck strcat wordfreq prodcons

N = 11

EXEFILES = $(foreach lang,$(EXELANGS),$(addprefix $(lang)/, $(addsuffix $(EXE),$(BENCHMARKS))))
JAVAFILES = $(foreach lang,$(JAVALANGS),$(addprefix $(lang)/, $(addsuffix .class,$(BENCHMARKS))))
NOCHECKFILES = $(foreach lang,cyclone cyclone-port,$(addprefix nocheck-$(lang)/, $(addsuffix $(EXE),$(BENCHMARKS))))
DIFFFILES = $(foreach lang,cyclone cyclone-port,$(addsuffix -$(lang).diff, $(BENCHMARKS)))
CYCHIST=cychist$(EXE)
CATN=catn$(EXE)

NOGC_EXEFILES := $(foreach lang,cyclone cyclone-port,$(addprefix $(lang)/, $(addsuffix $(EXE),$(NOGC_BENCHMARKS)))) $(addprefix cyclone/, $(addsuffix $(EXE),$(NOGC_OPT_BENCHMARKS)))
NOGC_EXEFILES += $(addprefix nocheck-,$(NOGC_EXEFILES))

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

diff: 
	export SRCS="$(DIFF_BENCHMARKS)"; \
	export HDRS=simple_hash; \
	export TESTNAME=shootout; \
	./diff.pl

count:
	wc -l $(addsuffix .gcc, $(BENCHMARKS))

clean:
	$(RM) $(EXEFILES) $(NOCHECKFILES) $(JAVAFILES)
	$(RM) $(CYCHIST) $(CATN)
	$(RM) *.diff */*.class *.eps

$(CYCHIST): cychist.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ -lm

%.eps: %.rout makejgraph
	cat $< | ./makejgraph | jgraph > $@

foo.diff: 
	SRCS=$(DIFF) gcc/$*.gcc cyclone/$*.cyc > $@

%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^ $(LDFLAGS)

java/%.java: java-start/%.java
	./inserttimer < $^ > $@

%.class: %.java
	@(cd `dirname $^`; CLASSPATH=$CLASSPATH:../jakarta-oro-2.0.8.jar javac `basename $^`; echo javac $^)

# Cyclone targets

ifndef REC
$(NOGC_EXEFILES):
	$(MAKE) REC=X "CYC_FLAGS=$(CYC_FLAGS) -nogc" $@

%/prodcons$(EXE):
	$(MAKE) REC=X "LDFLAGS=$(LDFLAGS) -lpthread" $@
endif

nocheck-cyclone/%$(EXE): cyclone/%.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

nocheck-cyclone-port/%$(EXE): cyclone-port/%.cyc
	$(CYCC) --nochecks $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
