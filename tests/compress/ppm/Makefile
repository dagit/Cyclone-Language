
# Note: We're not doing anything with t.cyc t2.cyc t3.cyc t4.cyc 
#       testcases.cyc and toplev.cyc.  Should we be?

include ../../../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../../../bin/$(CYCCOMP)

CYCBISON=../../../bin/cycbison.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CFLAGS=-g -fwritable-strings -I../../.. 

CYC_FLAGS=-I ../../../lib -tc -toc -pp

LIB=../../../bin/$(OUT_PREFIX)$(CYCLIB)

encode: bit_output.o arithmetic_encode.o adaptive_model.o encode.o
	gcc -g -o encode bit_output.o arithmetic_encode.o adaptive_model.o encode.o  ../../../bin/cyclib.a ../../../bin/gc.a

decode: bit_input.o arithmetic_decode.o adaptive_model.o decode.o
	gcc -g -o decode bit_input.o arithmetic_decode.o adaptive_model.o decode.o  ../../../bin/cyclib.a ../../../bin/gc.a

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $< 

clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), hello.o cyctest.o cyctest.c foo_tab.* foo.o foo.c *.exe)

clean:
	rm -f *.o *.c encode decode
