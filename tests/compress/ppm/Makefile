
# FIX: somehow we cause gcc to go into an infinite loop on model.c ??

# Note: we use the compiler in lib and src, not in bin

CYCDIR := ../../..
CYCCOMP := cyclone.exe

# include $(CYCDIR)/Makefile.inc

CYCC=$(CYCDIR)/src/$(CYCCOMP)

CYCBISON=$(CYCDIR)/bin/cycbison.exe

CYC_FLAGS=-I$(CYCDIR)/lib -B$(CYCDIR)/lib -pp -v

encode: arithcode.o model.o encode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

decode: arithcode.o model.o decode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

clean:
	rm -f *.o *.c *.exe encode decode
