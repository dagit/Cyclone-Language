CYCDIR = ../..

include $(CYCDIR)/Makefile.inc

CYCC = $(CYCDIR)/bin/$(CYCCOMP)

# Not working yet: echo moments
# Not implemented at all: lists methcall objinst prodcons regexmatch
#  spellcheck strcat wordfreq
BENCHMARKS = ackermann ary3 except fibo hash hash2 heapsort\
 hello matrix nestedloop random reversefile sieve sumcol wc

EXEFILES = $(addsuffix $(EXE), $(BENCHMARKS))
GCCEXEFILES = $(addprefix gcc-, $(EXEFILES))

all: $(EXEFILES) $(GCCEXEFILES)

bench: all
	bash -c "time -p ./ackermann 8 >/dev/null"
	bash -c "time -p ./gcc-ackermann 8 >/dev/null"
	bash -c "time -p ./ary3 7000 >/dev/null"
	bash -c "time -p ./gcc-ary3 7000 >/dev/null"
	bash -c "time -p ./fibo 32 >/dev/null"
	bash -c "time -p ./gcc-fibo 32 >/dev/null"
	bash -c "time -p ./hash 80000 >/dev/null"
	bash -c "time -p ./gcc-hash 80000 >/dev/null"
	bash -c "time -p ./hash2 150 >/dev/null"
	bash -c "time -p ./gcc-hash2 150 >/dev/null"
	bash -c "time -p ./heapsort 80000 >/dev/null"
	bash -c "time -p ./gcc-heapsort 80000 >/dev/null"
	bash -c "time -p ./hello 200 >/dev/null"
	bash -c "time -p ./gcc-hello 200 >/dev/null"
	bash -c "time -p ./matrix 300 >/dev/null"
	bash -c "time -p ./gcc-matrix 300 >/dev/null"
	bash -c "time -p ./nestedloop 16 >/dev/null"
	bash -c "time -p ./gcc-nestedloop 16 >/dev/null"
	bash -c "time -p ./random 900000 >/dev/null"
	bash -c "time -p ./gcc-random 900000 >/dev/null"
	bash -c "time -p ./reversefile 20 <reversefile-Input >/dev/null"
	bash -c "time -p ./gcc-reversefile 20 <reversefile-Input >/dev/null"
	bash -c "time -p ./sieve 900 >/dev/null"
	bash -c "time -p ./gcc-sieve 900 >/dev/null"
	bash -c "time -p ./sumcol 1000 <sumcol-Input >/dev/null"
	bash -c "time -p ./gcc-sumcol 1000 <sumcol-Input >/dev/null"
	bash -c "time -p ./wc 2000 <wc-Input >/dev/null"
	bash -c "time -p ./gcc-wc 2000 <wc-Input >/dev/null"

clean:
	$(RM) $(EXEFILES)
	$(RM) $(GCCEXEFILES)

gcc-%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^ -lm

%$(EXE): %.cyc
	$(CYCC) $(CYCFLAGS) -o $@ $^ -lm
