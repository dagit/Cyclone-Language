
# We assume the library exists -- it's your responsibility to
# make that directory if it's not.  (The Makefile in the parent
# directory does this.)

CYCDIR := ..

include $(CYCDIR)/Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC := $(CYCDIR)/bin/$(CYCCOMP)

CYCLEX := $(CYCDIR)/bin/cyclex$(EXE)
CYCBISON := $(CYCDIR)/bin/cycbison$(EXE)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

ifdef PROFILE_REGIONS
PROF_RGN_FLAG := -DCYC_REGION_PROFILE
else
PROF_RGN_FLAG := 
endif

ifdef MS
TOC_FLAG = -tovc
else
TOC_FLAG =
endif

LC_FLAG =

CYC_FLAGS := -I$(CYCDIR)/lib -B$(CYCDIR)/lib -save-c $(LC_FLAG) $(TOC_FLAG) $(PROF_RGN_FLAG)

LIBR:=$(CYCDIR)/lib/$(OUT_PREFIX)$(CYCLIB)

CYC_SRCS := $(addsuffix .cyc, $(CYCLONE_SRCS))

all: $(OUT_PREFIX)$(CYCCOMP)

# Note: although cyclone now automatically adds CYCLIB to the command
# line when calling gcc, we leave in an explicit $(LIBR) below so that
# we get the $(OUT_PREFIX) version instead of $(CYCDIR)/lib/cyclib.a
$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIBR)
ifdef MS
	cl /Fe$@ $(CYCDIR)/lib/snprintf.obj $^ $(CYCDIR)/bin/cyc-lib/gc.lib
else
	$(CYCC) $(LC_FLAG) -B$(CYCDIR)/lib -o $@ $^ $(LDFLAGS)
#	gcc -g -o $@ $^ $(CYCDIR)/bin/cyc-lib/gc.a $(LDFLAGS)
#	gcc -o $@ $^ $(CYCDIR)/bin/cyc-lib/gc_pg.a -pg $(LDFLAGS)
endif
# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lex.o : $(OUT_PREFIX)lex.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lex.cyc : lex.cyl $(CYCLEX)
	$(CYCLEX) $< $@

$(OUT_PREFIX)parse_tab.o: $(OUT_PREFIX)parse_tab.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $< 
$(OUT_PREFIX)parse_tab.cyc: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@
#$(OUT_PREFIX)parse_tab.h: parse.y $(CYCBISON)
#	$(CYCBISON) -v -d $< -o $(OUT_PREFIX)parse_tab.cyc
parse_tab.h: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o parse_tab.cyc

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

ifdef MS
%.obj: %.c
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

# delete generated files with a particular prefix
clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix $(OUT_PREFIX), lex.cyc parse_tab.cyc parse_tab.h)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix *, lex.cyc parse_tab.cyc parse_tab.h)
	rm -f *~ *.output *.stackdump *.d

# Produce dependencies
# We don't use $(CYCC) here because this would create a circular
# dependency when OUT_PREFIX and CYCC are overridden on the command line
# Note we have to add cyc_include.h by hand since it only shows up in
# the C code
$(OUT_PREFIX)%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYCDIR)/lib $< \
	| sed 's/^\(.*\).o:/$(OUT_PREFIX)\1.o $(OUT_PREFIX)\1.d: $(CYCDIR)\/lib\/include\/cyc_include.h/' > $@

# We need this as well as the above to handle $(OUT_PREFIX)lex.d,
# which depends on $(OUT_PREFIX)lex.cyc, not lex.cyc
%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYCDIR)/lib $< \
	| sed 's/^\(.*\).o:/\1.o \1.d: $(CYCDIR)\/lib\/include\/cyc_include.h/' > $@

# Include the dependencies unless we are doing a clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_SRCS)))
endif
endif

