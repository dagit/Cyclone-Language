
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
ifdef CHECK_STACK
ifdef UP_STACK
override CYC_FLAGS += $(LC_FLAG) -save-c -pp -g -DCHECK_STACK -DUP_STACK
else
override CYC_FLAGS += $(LC_FLAG) -save-c -pp -O2 -DCHECK_STACK
endif
else
override CYC_FLAGS += $(LC_FLAG) -save-c -pp -O2
endif
#override CYC_FLAGS += $(LC_FLAG) -save-c -pp -I../../lib -B../../lib -g

LIB=../../bin/cyc-lib/$(OUT_PREFIX)$(CYCLIB)

all: $(CYCC) $(LIB) \
  smlng$(EXE) generate$(EXE) validate$(EXE) decorate$(EXE) stack$(EXE)

GCC_LIBS = ../../bin/cyc-lib/libcyc.a ../../bin/cyc-lib/gc.a

#smlng$(EXE): smlng.o tag_elim.o timer.o buffer.o parser.o flat.o
smlng$(EXE): smlng.o tag_elim.o timer.o buffer2.o parser.o flat.o dynamic.o
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

stack$(EXE): stack.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $< 

smlng.cyc: smlng.h tag_elim.h timer.h buffer.h parser.h
buffer2.cyc: buffer.h
buffer.cyc: buffer.h
parser.cyc: parser.h buffer.h smlng.h

clean:
	$(RM) smlng.o tag_elim.o timer.o buffer2.o buffer.o parser.o generate.o validate.o decorate.o flat.o dynamic.o stack.o
	$(RM) smlng.c tag_elim.c timer.c buffer2.c buffer.c parser.c generate.c validate.c decorate.c flat.c dynamic.c stack.c
	$(RM) *~ *.exe *.output *.stackdump
	$(RM) smlng generate validate decorate stack
