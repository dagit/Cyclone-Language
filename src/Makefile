include ../Makefile.inc

ifndef ARCH
$(error "Must have ARCH variable defined to properly compile")
else
ifndef TARGET
TARGET := $(ARCH)
endif
endif

# CYC_BIN_PATH := $(CYCDIR)/src
# CYC_LIB_PATH := $(CYCDIR)/lib
# CYC_INC_PATH := $(CYCDIR)/lib

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
APROF_FLAG := -pa
LDFLAGS = -pa
else
APROF_FLAG := 
endif

ifdef MS
TOC_FLAG = -tovc
else
TOC_FLAG =
endif

LC_FLAG =

# for cross-compiling
CPP_FLAG := -use-cpp '$(CYCDIR)/config/cyccpp $(CYCDIR)/config/arch/$(ARCH) $(CYCDIR)/config/arch/$(TARGET)'

CYC_FLAGS += -I$(CYC_INC_PATH) -I. -B$(CYC_LIB_PATH) -save-c $(LC_FLAG) $(TOC_FLAG) $(APROF_FLAG) $(CPP_FLAG)
#CYC_FLAGS += -g -I$(CYC_INC_PATH) -I. -B$(CYC_LIB_PATH) -save-c $(LC_FLAG) $(TOC_FLAG) $(APROF_FLAG)
#CYC_FLAGS += -I$(CYC_INC_PATH) -I. -B$(CYC_LIB_PATH) -pg -save-c $(LC_FLAG) $(TOC_FLAG) $(APROF_FLAG)

LIBR:=$(CYC_LIB_PATH)/$(OUT_PREFIX)$(CYCLIB)

CYC_SRCS := $(addsuffix .cyc, $(CYCLONE_SRCS))

all: $(OUT_PREFIX)$(CYCCOMP)

# Note: although cyclone now automatically adds CYCLIB to the command
# line when calling gcc, we leave in an explicit $(LIBR) below so that
# we get the $(OUT_PREFIX) version instead of $(CYCDIR)/lib/cyclib.a
$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIBR) install_path.$(O)
ifdef MS
	cl /Fe$@ $(CYC_LIB_PATH)/snprintf.obj $^ $(CYCDIR)/bin/cyc-lib/gc.lib
else
	$(CYCC) $(LC_FLAG) -B$(CYC_LIB_PATH) -o $@ $^ $(LDFLAGS)
#	gcc -g -o $@ $^ $(CYCDIR)/bin/cyc-lib/gc.a $(LDFLAGS)
#	gcc -o $@ $^ $(CYCDIR)/bin/cyc-lib/gc_pg.a -pg $(LDFLAGS)
endif
	$(RM) install_path.c install_path.$(O)

install_path.c:
	@(echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";") > $@

src: $(addprefix $(OUT_PREFIX), $(C_SRCS))

# For machine-generated we need special targets (the .cyc is prefixed)
# $(OUT_PREFIX)lex.o : $(OUT_PREFIX)lex.cyc $(CYCC) $(CYCINC_H)
# 	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lex.cyc : lex.cyl $(CYCLEX)
	$(CYCLEX) $< $@

# $(OUT_PREFIX)parse_tab.o: $(OUT_PREFIX)parse_tab.cyc $(CYCC) $(CYCINC_H)
# 	$(CYCC) $(CYC_FLAGS) -c $< 
$(OUT_PREFIX)parse_tab.cyc: parse.y $(CYCBISON)
	$(CYCBISON) -d $< -o $@
#$(OUT_PREFIX)parse_tab.h: parse.y $(CYCBISON)
#	$(CYCBISON) -v -d $< -o $(OUT_PREFIX)parse_tab.cyc
parse_tab.h: parse.y $(CYCBISON)
	$(CYCBISON) -d $< -o parse_tab.cyc

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o $(OUT_PREFIX)%.c: %.cyc $(CYCC) $(CYCINC_H)
	@if [ -f ".target$(OUT_PREFIX)" ]; then \
	if [ "`cat .target$(OUT_PREFIX)`" != "$(TARGET)" ]; then \
	echo "unknown" > .target$(OUT_PREFIX); \
	fi; \
	else echo $(TARGET) > .target$(OUT_PREFIX); fi
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

ifdef MS
%.obj: %.c $(CYCINC_H)
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $<
endif

# delete generated files with a particular prefix
clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix $(OUT_PREFIX), lex.cyc parse_tab.cyc parse_tab.h)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix *, lex.cyc parse_tab.cyc parse_tab.h)
	rm -f *~ *.output *.stackdump *.d amon.out .target*

# Produce dependencies
# We don't use $(CYCC) here because this would create a circular
# dependency when OUT_PREFIX and CYCC are overridden on the command line
$(OUT_PREFIX)%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYC_INC_PATH) $< > $@

# We need this as well as the above to handle $(OUT_PREFIX)lex.d,
# which depends on $(OUT_PREFIX)lex.cyc, not lex.cyc
%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -MG -I$(CYC_INC_PATH) $< > $@

# Include the dependencies unless we are doing a clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
-include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_SRCS)))
endif
endif
