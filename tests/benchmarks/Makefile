include ../../Makefile.inc

PROGS := fib hanoi shuffle nsieve tfftdp

CYCFLAGS := -lm

ifndef CYGWIN
ifneq ($(OS),Windows_NT)
ifneq ($(OSTYPE),cygwin)
CFLAGS   := -lm
endif
endif
endif

CYC_BIN_PATH := $(CYCDIR)/bin
CYC_LIB_PATH := $(CYCDIR)/bin/cyc-lib
CYC_INC_PATH := $(CYCDIR)/include
LC_FLAG=
CYCC := $(CYC_BIN_PATH)/$(CYCCOMP)

CYC_FLAGS+= $(LC_FLAG) -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -O3
CFLAGS   := $(CFLAGS)   -O3

#########################################################

CYCPROGS := $(addsuffix $(EXE), $(PROGS))
CPROGS := $(addsuffix $(EXE), $(addsuffix -orig, $(PROGS)))
BENCH  := $(addprefix bench-, $(PROGS))
RUN    := $(addprefix run-,   $(PROGS))

ALL := $(CYCPROGS) $(CPROGS)

#########################################################

world: $(ALL)

bench: $(BENCH)

clean:
	rm -f $(ALL)

#########################################################

%-orig$(EXE): %-orig.c
	$(CC) $(CFLAGS) -Wall -o $@ $<

%$(EXE): %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<

##JGM: for some reason, I needed this...
#fib$(EXE) : fib.cyc $(CYCC)
#	$(CYCC) $(CYCFLAGS) -o $@ $<

#I tried this to get the labels to work automatically...
#
#bench-%: % %-orig echo-% run-%
#	@
#
#echo-%: %
#	@echo Benchmarking $^

bench-%: % %-orig run-%
	@

#########################################################

run-hanoi:
	@echo Towers of Hanoi
	time ./hanoi-orig 23 > /dev/null
	time ./hanoi      23 > /dev/null
	@echo

run-fib:
	@echo Fibonacci Numbers
	time ./fib-orig 37 > /dev/null
	time ./fib      37 > /dev/null
	@echo

run-shuffle:
	@echo Psuedorandom Numbers
	time ./shuffle-orig 156 > /dev/null
	time ./shuffle      156 > /dev/null
	@echo

run-nsieve:
	@echo Sieve of Eratosthenes
	time ./nsieve-orig 0 > /dev/null
	time ./nsieve      0 > /dev/null
	@echo

run-tfftdp:
	@echo Double-precision FFT
	time ./tfftdp-orig > /dev/null
	time ./tfftdp      > /dev/null
	@echo
