
include ../Makefile.inc

# Override this variable to use a bootstrapped compiler (you may want
# an absolute path)
CYCC:=../bin/$(CYCCOMP)

CYCLEX:=../bin/cyclex.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX:=

ifdef PROFILE_REGIONS
PROF_RGN_FLAG := -D CYC_REGION_PROFILE
else
PROF_RGN_FLAG := 
endif

CFLAGS:=-fwritable-strings -I.. $(PROF_RGN_FLAG)
#CFLAGS:=-g -fwritable-strings -I.. $(PROF_RGN_FLAG)
#CFLAGS:=-pg -fwritable-strings -I.. $(PROF_RGN_FLAG)
#CFLAGS:=-O -fwritable-strings -I.. $(PROF_RGN_FLAG)

CYC_LIBS:=$(addsuffix .cyc, $(CYCLONE_LIBS))

ifdef MS
TOC_FLAG = -toc -tovc
else
TOC_FLAG = -toc
endif

# uh, the precore_c.h building doesn't work with the ugly printer :-)
CYC_FLAGS_PP := -tc $(TOC_FLAG) $(CYC_REGION_PROFILE) -pp

#CYC_FLAGS:= -tc $(TOC_FLAG) -pp -I . 
CYC_FLAGS:= -tc $(TOC_FLAG) -up -I . 

all: $(OUT_PREFIX)$(CYCLIB) xml/libxml.a

$(OUT_PREFIX)$(CYCLIB): $(addprefix $(OUT_PREFIX), $(O_LIBS) $(RUNTIME).$(O))
ifdef MS
	lib /out:$@ $^
else
	-rm -f $@
	ar qc $@ $^
	ranlib $@
endif

precore_c.h: precore.h $(CYCC)
	$(CYCC) $(CYC_FLAGS_PP) -D _CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs $< > $@

xml/libxml.a: 
	$(MAKE) -C xml CYCC=../$(CYCC)

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lineno.c : $(OUT_PREFIX)lineno.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<
$(OUT_PREFIX)lineno.cyc : lineno.cyl $(CYCLEX)
	$(CYCLEX) $< $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<

ifdef MS
%.obj: %.c
	cl /I.. /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

# We need a special target becaue the .c file won't have the OUT_PREFIX
$(OUT_PREFIX)$(RUNTIME).$(O): $(RUNTIME).c precore_c.h 
ifdef MS
	cl /I.. /Dinline=__inline /c /w /nologo /Fo$@ $(RUNTIME).c
else
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(RUNTIME).c
endif

# delete generated files with a particular prefix
clean_prefix: 
	rm -f $(addprefix $(OUT_PREFIX), $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix $(OUT_PREFIX), precore_c.h lineno.cyc)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix *, precore_c.h lineno.cyc)
	rm -f *~ *.output *.stackdump
	$(MAKE) clean -C xml

# dependencies -- maintained by hand for now

TYPES_H     := sys/features.h 
PRECORE_H   := precore.h $(TYPES_H)
CORE_H      := core.h       $(PRECORE_H)
STDIO_H     := $(CORE_H)
LIST_H      := list.h       $(CORE_H)
ARRAY_H     := array.h      $(CORE_H) $(LIST_H)
BUFFER_H    := buffer.h     $(CORE_H) $(STDIO_H)
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
CONFIG_H    := config.h
STDDEF_H    := stddef.h
TIME_H      := time.h $(CONFIG_H) $(CORE_H)
SYS_REENT_H := sys/reent.h $(TIME_H)
STDLIB_H    := $(SYS_REENT_H) $(CONFIG_H)
CTYPE_H     := ctype.h $(CONFIG_H)
MACHINE_TYPES_H := machine/types.h
SYS_TYPES_H := sys/types.h $(STDDEF_H) $(MACHINE_TYPES_H)
SYS_SIGNAL_H := sys/signal.h $(CONFIG_H) sys/features.h $(SYS_TYPES_H)
SIGNAL_H    := signal.h $(CONFIG_H) $(SYS_SIGNAL_H)


$(OUT_PREFIX)arg.c: $(ARG_H) $(STRING_H)
$(OUT_PREFIX)bitvec.c: $(BITVEC_H)
$(OUT_PREFIX)buffer.c: $(BUFFER_H) $(STRING_H)
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
$(OUT_PREFIX)string.c: $(CORE_H) $(LIST_H) $(CTYPE_H) $(STRING_H) $(MATH_H)
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
$(OUT_PREFIX)errno.c: $(CORE_H) $(ERRNO_H) $(CONFIG_H)
$(OUT_PREFIX)stdlib.c: $(CORE_H) $(STDLIB_H)
$(OUT_PREFIX)time.c: $(CORE_H) $(TIME_H)
