
CYCDIR := ..

include $(CYCDIR)/Makefile.inc

# Override this variable to use a bootstrapped compiler (you may want
# an absolute path)
CYCC:=$(CYCDIR)/bin/$(CYCCOMP)

CYCLEX:=$(CYCDIR)/bin/cyclex$(EXE)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX:=

ifdef PROFILE_REGIONS
PROF_RGN_FLAG := -DCYC_REGION_PROFILE
else
PROF_RGN_FLAG := 
endif

CFLAGS:=-fwritable-strings -I$(CYCDIR) $(PROF_RGN_FLAG)
#CFLAGS:=-g -fwritable-strings -I$(CYCDIR) $(PROF_RGN_FLAG)
#CFLAGS:=-pg -fwritable-strings -I$(CYCDIR) $(PROF_RGN_FLAG)
#CFLAGS:=-O -fwritable-strings -I$(CYCDIR) $(PROF_RGN_FLAG)

CYC_LIBS:=$(addsuffix .cyc, $(CYCLONE_LIBS))

ifdef MS
TOC_FLAG = -tovc
else
TOC_FLAG =
endif

LC_FLAG =

CYC_FLAGS := -I. -B. -save-c $(LC_FLAG) $(PROF_RGN_FLAG) $(TOC_FLAG)

all: $(OUT_PREFIX)$(CYCLIB) xml/libxml.a

$(OUT_PREFIX)$(CYCLIB): $(addprefix $(OUT_PREFIX), $(O_LIBS) $(RUNTIME).$(O))
ifdef MS
	lib /out:$@ $^
else
	-rm -f $@
ifeq ($(LC_FLAG),)
	ar rcs $@ $^
else
	$(CYCC) $(LC_FLAG) -o $@ $^
endif
endif

precore_c.h: precore.h $(CYCC)
	cp precore.h precore.cyc
	$(CYCC) -toc -pp $(CYC_REGION_PROFILE) -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -o $@ precore.cyc
	rm -f precore.cyc

xml/libxml.a: 
	$(MAKE) -C xml CYCC=$(CYCDIR)/$(CYCC)

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lineno.o : $(OUT_PREFIX)lineno.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lineno.cyc : lineno.cyl $(CYCLEX)
	$(CYCLEX) $< $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

ifdef MS
%.obj: %.c
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

# We need a special target because the .c file won't have the OUT_PREFIX
$(OUT_PREFIX)$(RUNTIME).$(O): $(RUNTIME).c precore_c.h 
ifdef MS
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $(RUNTIME).c
else
	$(CC) -c -o $@ -include include/cyc_include.h $(CPPFLAGS) $(CFLAGS) $(RUNTIME).c
endif

# delete generated files with a particular prefix
clean_prefix: 
	rm -f $(addprefix $(OUT_PREFIX), $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix $(OUT_PREFIX), precore_c.h lineno.cyc)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix *, precore_c.h lineno.cyc)
	rm -f *~ *.output *.stackdump *.d
	$(MAKE) clean -C xml

# Produce dependencies
# Note we have to add cyc_include.h by hand since that only shows up in
# the C code.
$(OUT_PREFIX)%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -I. $< \
	| sed 's/^\(.*\).o:/$(OUT_PREFIX)\1.o $(OUT_PREFIX)\1.d: include\/cyc_include.h/' > $@

# Include the dependencies unless we are doing a clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_LIBS)))
endif
endif
