
# FIX: somehow we cause gcc to go into an infinite loop on model.c ??

# Note: we use the compiler in lib and src, not in bin

CYCDIR := ../../..

include $(CYCDIR)/Makefile.inc

CYCC=$(CYCDIR)/bin/$(CYCCOMP)

CYC_FLAGS+=-I$(CYCDIR)/lib -B$(CYCDIR)/lib 

encode$(EXE): arithcode.o model.o encode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^ -lm

decode$(EXE): arithcode.o model.o decode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^ -lm

%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

clean:
	rm -f *.o *.c *.exe encode decode
