include ../Makefile.inc

CYC_BIN_PATH := $(CYCDIR)/bin
CYC_LIB_PATH := $(CYCDIR)/lib
CYC_INC_PATH := $(CYCDIR)/lib

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC := $(CYC_BIN_PATH)/$(CYCCOMP)
CYCLEX := $(CYC_BIN_PATH)/cyclex$(EXE)
CYCBISON := $(CYC_BIN_PATH)/cycbison$(EXE)

CYCINC_H := $(CYC_LIB_PATH)/include/cyc_include.h

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

ifdef PROFILE_REGIONS
APROF_FLAG:=-pa
LDFLAGS=-pa
else
APROF_FLAG:= 
endif

ifdef MS
TOVC_FLAG=-tovc
else
TOVC_FLAG=
endif

LC_FLAG=
G_FLAG=
PG_FLAG=

CYC_FLAGS += -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -save-c $(G_FLAG) $(PG_FLAG) $(LC_FLAG) $(TOVC_FLAG) $(APROF_FLAG) $(CPP_FLAG)

LIBR:=$(CYC_LIB_PATH)/$(OUT_PREFIX)$(CYCLIB)

CYC_SRCS := $(addsuffix .cyc, $(CYCLONE_SRCS))

all: $(OUT_PREFIX)$(CYCCOMP)

# Note: although cyclone now automatically adds CYCLIB to the command
# line when calling gcc, we leave in an explicit $(LIBR) below so that
# we get the $(OUT_PREFIX) version instead of $(CYCDIR)/lib/cyclib.a
# FIX? Currently, the compiler does not use a profile-aware version of the gc!
$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIBR) install_path.$(O)
ifdef MS
#FIX: Ignore fewer flags.
	cl /Fe$@ $(CYC_LIB_PATH)/snprintf.obj $^ $(CYCDIR)/bin/cyc-lib/gc.lib
else
	$(CYCC) $(G_FLAG) $(PG_FLAG) $(LC_FLAG) -B$(CYC_LIB_PATH) -o $@ $^ $(LDFLAGS)
endif
	$(RM) install_path.c install_path.$(O)

install_path.c:
	@(echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";") > $@

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lex.o : $(OUT_PREFIX)lex.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lex.cyc : lex.cyl $(CYCLEX)
	$(CYCLEX) $< $@

$(OUT_PREFIX)parse_tab.o: $(OUT_PREFIX)parse_tab.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) $(CYC_FLAGS) -c $< 
$(OUT_PREFIX)parse_tab.cyc: parse.y $(CYCBISON)
	$(CYCBISON) -d $< -o $@
#$(OUT_PREFIX)parse_tab.h: parse.y $(CYCBISON)
#	$(CYCBISON) -v -d $< -o $(OUT_PREFIX)parse_tab.cyc
parse_tab.h: parse.y $(CYCBISON)
	$(CYCBISON) -d $< -o parse_tab.cyc

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o $(OUT_PREFIX)%.c: %.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

ifdef MS
# FIX: ignore fewer flags
%.obj: %.c $(CYCINC_H)
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $<
endif

# for cross-compiling -- uses very little of the above stuff
ifdef TARGET
ifndef ARCH
$(error "ARCH must be defined if TARGET is. TARGET is neeeded only for cross-compiling.)");
endif
ifdef OUT_PREFIX
$(error "There is no reason to cross-compile with an OUT_PREFIX.")
endif
CONFIGDIR := $(CYCDIR)/config
TARGETDIR := ../$(TARGET)/

$(TARGET): $(addprefix $(TARGETDIR), $(C_SRCS))

$(TARGETDIR)%.c: %.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) -D_PLATFORM_INCLUDES_="\"arch/$(TARGET).h\"" -I$(CYC_INC_PATH) -toc $(TOVC_FLAG) -save-c -use-cpp '$(CONFIGDIR)/cyccpp $(CONFIGDIR)/arch/$(ARCH) $(CONFIGDIR)/arch/$(TARGET)' -o $@ $<

endif

# delete generated files with a particular prefix
clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix $(OUT_PREFIX), lex.cyc parse_tab.cyc parse_tab.h)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix *, lex.cyc parse_tab.cyc parse_tab.h)
	rm -f *~ *.output *.stackdump *.d amon.out .target* install_path.c

# Produce dependencies
# We don't use $(CYCC) here because this would create a circular
# dependency when OUT_PREFIX and CYCC are overridden on the command line
$(OUT_PREFIX)%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYC_INC_PATH) $(CYC_FLAGS) $< > $@

# We need this as well as the above to handle $(OUT_PREFIX)lex.d,
# which depends on $(OUT_PREFIX)lex.cyc, not lex.cyc
%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYC_INC_PATH) $(CYC_FLAGS) $< > $@

# Include the dependencies unless we are doing a clean or a cross-compile.
# (For the latter, this could be a problem, but fresh builds are the norm.)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
ifndef $(TARGET)
-include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_SRCS)))
endif
endif
endif