
# FIX: somehow we cause gcc to go into an infinite loop on model.c ??

# Note: we use the compiler in lib and src, not in bin

CYCDIR := ../../..

include $(CYCDIR)/Makefile.inc

CYCC=$(CYCDIR)/bin/$(CYCCOMP)

override CYC_FLAGS += -I$(CYCDIR)/lib -B$(CYCDIR)/lib 

all: encode$(EXE) decode$(EXE)

encode$(EXE): arithcode.o model.o encode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

decode$(EXE): arithcode.o model.o decode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

clean:
	rm -f *.o *.c *.exe encode decode
