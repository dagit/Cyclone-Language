
# Note: this tests the compiler and library in ../src and ../lib,
# not the compiler in ../bin

# Note: We're not doing anything with t.cyc t2.cyc t3.cyc t4.cyc 
#       testcases.cyc and toplev.cyc.  Should we be?

include ../../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../../bin/$(CYCCOMP)

CYCBISON=../../bin/cycbison$(EXE)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

LC_FLAG=
#CYC_FLAGS= $(LC_FLAG) -save-c -pp -I../../lib -B../../lib -O2
CYC_FLAGS= $(LC_FLAG) -save-c -pp -I../../lib -B../../lib -g

LIB=../../bin/cyc-lib/$(OUT_PREFIX)$(CYCLIB)

all: $(CYCC) $(LIB) \
  smlng$(EXE) generate$(EXE) validate$(EXE) decorate$(EXE)

GCC_LIBS = ../../bin/cyc-lib/libcyc.a ../../bin/cyc-lib/gc.a

smlng$(EXE): smlng.o tag_elim.o timer.o buffer.o parser.o flat.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
#	gcc -g -o $@ $^ $(GCC_LIBS) $(LDFLAGS)

generate$(EXE): generate.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

validate.o: scanner contexts.h

validate$(EXE): validate.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

decorate.o: scanner contexts.h

decorate$(EXE): decorate.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $< 

smlng.cyc: smlng.h tag_elim.h timer.h buffer.h parser.h
buffer.cyc: buffer.h
parser.cyc: parser.h buffer.h smlng.h

clean:
	rm -f smlng.o tag_elim.o timer.o buffer.o parser.o generate.o validate.o flat.o
	rm -f smlng.c tag_elim.c timer.c buffer.c parser.c generate.c validate.c flat.c
	rm -f *~ *.exe *.output *.stackdump
