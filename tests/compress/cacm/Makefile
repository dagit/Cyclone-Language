
# Note: we use the compiler in lib and src, not bin

CYCDIR := ../../..
include $(CYCDIR)/Makefile.inc

CYCC := $(CYCDIR)/src/$(CYCCOMP)

# FIX: if -pp is not used, there is an error in ugly-printing that
# leaves an unterminated comment
CYC_FLAGS := -pp -I$(CYCDIR)/lib -B$(CYCDIR)/lib

encode$(EXE): bit_output.o arithmetic_encode.o adaptive_model.o encode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

decode$(EXE): bit_input.o arithmetic_decode.o adaptive_model.o decode.o
	$(CYCC) $(CYC_FLAGS) -g -o $@ $^

%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

clean:
	rm -f *.o *.c *.exe encode decode
