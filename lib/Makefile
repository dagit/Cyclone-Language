
include ../Makefile.inc

# Override this variable to use a bootstrapped compiler (you may want
# an absolute path)
CYCC:=../bin/$(CYCCOMP)

CYCLEX:=../bin/cyclex.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX:=

CFLAGS:=-fwritable-strings -I..
#CFLAGS:=-g -fwritable-strings -I..
#CFLAGS:=-pg -fwritable-strings -I..
#CFLAGS:=-O -fwritable-strings -I..

CYC_LIBS:=$(addsuffix .cyc, $(CYCLONE_LIBS))

# uh, the precore_c.h building doesn't work with the ugly printer :-)
CYC_FLAGS_PP := -tc -toc -pp

#CYC_FLAGS:= -tc -toc -pp
CYC_FLAGS:= -tc -toc -up

all: $(OUT_PREFIX)$(CYCLIB)

$(OUT_PREFIX)$(CYCLIB): $(addprefix $(OUT_PREFIX), $(O_LIBS) $(RUNTIME).o)
	-rm -f $@
	ar qc $@ $^

precore_c.h: precore.h $(CYCC)
	$(CYCC) $(CYC_FLAGS_PP) -nocyc -noremoveunused -noexpandtypedefs $< | grep -v __sFILE > $@

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lineno.c : $(OUT_PREFIX)lineno.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<
$(OUT_PREFIX)lineno.cyc : lineno.cyl $(CYCLEX)
	$(CYCLEX) $< $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<

# We need a special target becaue the .c file won't have the OUT_PREFIX
$(OUT_PREFIX)$(RUNTIME).o: $(RUNTIME).c precore_c.h 
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(RUNTIME).c

# delete generated files with a particular prefix
clean_prefix: 
	rm -f $(addprefix $(OUT_PREFIX), $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix $(OUT_PREFIX), precore_c.h lineno.cyc)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix *, precore_c.h lineno.cyc)
	rm -f *~ *.output *.stackdump

# dependencies -- maintained by hand for now
PRECORE_H   := precore.h
CORE_H      := core.h       $(PRECORE_H)
STDIO_H     := $(CORE_H)
LIST_H      := list.h       $(CORE_H)
ARRAY_H     := array.h      $(CORE_H) $(LIST_H)
BUFFER_H    := buffer.h     $(CORE_H) $(STDIO_H)
CHARACTER_H := character.h 
SLOWDICT_H  := slowdict.h   $(CORE_H) $(LIST_H)
DICT_H      := dict.h       $(CORE_H) $(LIST_H)
FILENAME_H  := filename.h   $(CORE_H)
FN_H        := fn.h         $(LIST_H)
HASHTABLE_H := hashtable.h
ID_H        := id.h         $(CORE_H) $(STDIO_H)
LEXING_H    := lexing.h     $(CORE_H) $(STDIO_H)
LINENO_H    := lineno.h     $(LIST_H)
PP_H        := pp.h         $(CORE_H) $(LIST_H) $(STDIO_H)
QUEUE_H     := queue.h
REF_H       := ref.h
ROPE_H      := rope.h       $(CORE_H) $(LIST_H)
SET_H       := set.h        $(LIST_H)
SPLAY_H     := splay.h      $(CORE_H) $(LIST_H)
STRING_H    := string.h     $(CORE_H) $(LIST_H) $(STDIO_H)
XARRAY_H    := xarray.h     $(CORE_H)
ARG_H       := arg.h        $(CORE_H) $(LIST_H)
POSITION_H  := position.h   $(CORE_H) $(LIST_H) $(LINENO_H)
BITVEC_H    := bitvec.h     $(LIST_H) $(SET_H)  
MATH_H      := math.h
ASSERT_H    := assert.h     $(CORE_H)
ERRNO_H     := errno.h      

$(OUT_PREFIX)arg.c: $(ARG_H) $(STRING_H)
$(OUT_PREFIX)bitvec.c: $(BITVEC_H)
$(OUT_PREFIX)buffer.c: $(BUFFER_H) $(STRING_H)
$(OUT_PREFIX)character.c:
$(OUT_PREFIX)core.c: $(CORE_H)
$(OUT_PREFIX)slowdict.c: $(CORE_H) $(LIST_H) $(SPLAY_H) $(SLOWDICT_H)
$(OUT_PREFIX)dict.c: $(DICT_H) $(CORE_H) $(LIST_H)
$(OUT_PREFIX)filename.c: $(FILENAME_H) $(STRING_H)
$(OUT_PREFIX)hashtable.c: $(LIST_H) $(CORE_H) $(HASHTABLE_H) $(STRING_H)
$(OUT_PREFIX)list.c: $(LIST_H)
$(OUT_PREFIX)queue.c: $(LIST_H) $(QUEUE_H)
$(OUT_PREFIX)ref.c: $(REF_H)
$(OUT_PREFIX)rope.c: $(ROPE_H) $(STRING_H)
$(OUT_PREFIX)set.c: $(LIST_H) $(SET_H)
$(OUT_PREFIX)splay.c: $(CORE_H) $(LIST_H) $(SPLAY_H)
$(OUT_PREFIX)string.c: $(CORE_H) $(LIST_H) $(CHARACTER_H) $(STRING_H) $(MATH_H)
$(OUT_PREFIX)lexing.c: $(CORE_H) $(LEXING_H) $(STRING_H)
$(OUT_PREFIX)id.c: $(CORE_H) $(STRING_H) $(LIST_H) $(ID_H)
$(OUT_PREFIX)xarray.c: $(CORE_H) $(XARRAY_H)
$(OUT_PREFIX)fn.c: $(FN_H)
$(OUT_PREFIX)array.c: $(CORE_H) $(LIST_H)
$(OUT_PREFIX)position.c: $(POSITION_H) $(STRING_H)
$(OUT_PREFIX)pp.c: $(CORE_H) $(LIST_H) $(STRING_H) $(HASHTABLE_H) $(FN_H) $(PP_H)
$(OUT_PREFIX)lineno.c: $(CORE_H) $(LEXING_H) $(STRING_H) $(LINENO_H) $(LEXING_H)
$(OUT_PREFIX)stdio.c: $(STDIO_H)
$(OUT_PREFIX)assert.c: $(ASSERT_H) $(CORE_H) $(STDIO_H)
$(OUT_PREFIX)errno.c: $(CORE_H) $(ERRNO_H)
