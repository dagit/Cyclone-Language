include ../../Makefile.inc

CYCC = $(CYCDIR)/bin/$(CYCCOMP)
CFLAGS = -O3
CYC_FLAGS = -O3

# Not working yet: echo moments
# Not implemented at all: lists methcall objinst prodcons regexmatch
#  spellcheck strcat wordfreq
#BENCHMARKS = ackermann ary3 except fibo hash hash2 heapsort\
# hello matrix moments nestedloop random reversefile sieve sumcol wc
BENCHMARKS = ackermann ary3 except fibo hash hash2 heapsort\
hello matrix nestedloop random reversefile sieve sumcol wc lists \
spellcheck

GCCEXEFILES = $(addsuffix $(EXE),$(addprefix gcc-, $(BENCHMARKS)))
EXEFILES = $(addsuffix $(EXE), $(BENCHMARKS))

all: $(EXEFILES) $(GCCEXEFILES)

TIME=/usr/bin/time

bench: all
	$(TIME) -p ./ackermann 8 >/dev/null
	$(TIME) -p ./gcc-ackermann 8 >/dev/null
	$(TIME) -p ./ary3 7000 >/dev/null
	$(TIME) -p ./gcc-ary3 7000 >/dev/null
	$(TIME) -p ./fibo 32 >/dev/null
	$(TIME) -p ./gcc-fibo 32 >/dev/null
	$(TIME) -p ./hash 80000 >/dev/null
	$(TIME) -p ./gcc-hash 80000 >/dev/null
	$(TIME) -p ./hash2 150 >/dev/null
	$(TIME) -p ./gcc-hash2 150 >/dev/null
	$(TIME) -p ./heapsort 80000 >/dev/null
	$(TIME) -p ./gcc-heapsort 80000 >/dev/null
	$(TIME) -p ./hello 200 >/dev/null
	$(TIME) -p ./gcc-hello 200 >/dev/null
	$(TIME) -p ./matrix 300 >/dev/null
	$(TIME) -p ./gcc-matrix 300 >/dev/null
	$(TIME) -p ./nestedloop 16 >/dev/null
	$(TIME) -p ./gcc-nestedloop 16 >/dev/null
	$(TIME) -p ./random 900000 >/dev/null
	$(TIME) -p ./gcc-random 900000 >/dev/null
	$(TIME) -p ./reversefile 20 <reversefile-Input >/dev/null
	$(TIME) -p ./gcc-reversefile 20 <reversefile-Input >/dev/null
	$(TIME) -p ./sieve 900 >/dev/null
	$(TIME) -p ./gcc-sieve 900 >/dev/null
	$(TIME) -p ./sumcol 1000 <sumcol-Input >/dev/null
	$(TIME) -p ./gcc-sumcol 1000 <sumcol-Input >/dev/null
	$(TIME) -p ./wc 2000 <wc-Input >/dev/null
	$(TIME) -p ./gcc-wc 2000 <wc-Input >/dev/null

clean:
	$(RM) $(EXEFILES)
	$(RM) $(GCCEXEFILES)

gcc-%$(EXE): %.gcc
	gcc $(CFLAGS) -o $@ -x c $^

%$(EXE): %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $^
