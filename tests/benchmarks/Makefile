include ../../Makefile.inc

CYCC := ../../bin/$(CYCCOMP)

PROGS := fib hanoi shuffle

#CYCFLAGS := -O3
#CFLAGS   := -O3
CYCFLAGS := 
CFLAGS   := 

#########################################################

CPROGS := $(addsuffix -orig, $(PROGS))
BENCH  := $(addprefix bench-, $(PROGS))
RUN    := $(addprefix run-,   $(PROGS))

ALL := $(PROGS) $(CPROGS)

#########################################################

world: $(ALL)

bench: $(BENCH)

clean:
	rm -f $(ALL)

#########################################################

%-orig: %-orig.c
	$(CC) $(CFLAGS) -Wall -o $@ $<

%: %.cyc
	$(CYCC) $(CYCFLAGS) -o $@ $<

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

