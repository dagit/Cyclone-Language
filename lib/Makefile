include ../Makefile.inc

ifndef ARCH
$(error "Must have ARCH variable defined to properly compile")
else
ifndef TARGET
TARGET := $(ARCH)
endif
endif

# use the installed compiler
CYC_BIN_PATH := $(CYCDIR)/bin

# but use the .h's in this directory
CYC_LIB_PATH := $(CYCDIR)/lib
CYC_INC_PATH := $(CYCDIR)/lib

# Override this variable to use a bootstrapped compiler (you may want
# an absolute path)
CYCC:=$(CYC_BIN_PATH)/$(CYCCOMP)
CYCLEX:=$(CYC_BIN_PATH)/cyclex$(EXE)
CYCINC_H := $(CYC_LIB_PATH)/include/cyc_include.h

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX:=

ifdef PROFILE_REGIONS
C_APROF_FLAG := -DCYC_REGION_PROFILE
CYC_APROF_FLAG := -pa
else
C_APROF_FLAG := 
CYC_APROF_FLAG := 
endif

CFLAGS:=-fwritable-strings -I$(CYCDIR) $(C_APROF_FLAG)

CYC_LIBS:=$(addsuffix .cyc, $(CYCLONE_LIBS))

ifdef MS
TOC_FLAG = -tovc
else
TOC_FLAG =
endif

LC_FLAG =

# for cross-compiling
CPP_FLAG := -use-cpp '$(CYCDIR)/config/cyccpp $(CYCDIR)/config/arch/$(ARCH) $(CYCDIR)/config/arch/$(TARGET)'

CYC_FLAGS := -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -save-c $(LC_FLAG) $(CYC_APROF_FLAG) $(TOC_FLAG) $(CPP_FLAG)
#CYC_FLAGS := -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -g -pp -save-c $(LC_FLAG) $(CYC_APROF_FLAG) $(TOC_FLAG)
#CYC_FLAGS := -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -pg -save-c $(LC_FLAG) $(CYC_APROF_FLAG) $(TOC_FLAG)

all: $(OUT_PREFIX)$(CYCLIB) nogc.a xml/libxml.a

src: $(addprefix $(OUT_PREFIX), $(C_SRCS))

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

nogc.a: nogc.o
	ar rcs $@ $^

precore_c.h: precore.h $(CYCC)
	cp precore.h precore.cyc
	$(CYCC) -toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -o $@ precore.cyc
	rm -f precore.cyc

xml/libxml.a: 
	$(MAKE) -C xml TARGET=$(TARGET)
#	$(MAKE) -C xml CYCC=$(CYCDIR)/$(CYCC)

# For machine-generated we need special targets (the .cyc is prefixed)
# $(OUT_PREFIX)lineno.o : $(OUT_PREFIX)lineno.cyc $(CYCC) $(CYCINC_H)
# 	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lineno.cyc : lineno.cyl $(CYCLEX) 
	$(CYCLEX) $< $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc $(CYCC) $(CYCINC_H)
	@if [ -f ".target$(OUT_PREFIX)" ]; then \
	if [ "`cat .target$(OUT_PREFIX)`" != "$(TARGET)" ]; then \
	echo "unknown" > .target$(OUT_PREFIX); \
	fi; \
	else echo $(TARGET) > .target$(OUT_PREFIX); fi
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

$(OUT_PREFIX)%.c: %.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) $(CYC_FLAGS) -save-c -toc -c -o $@ $<

ifdef MS
%.obj: %.c
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

# We need a special target because the .c file won't have the OUT_PREFIX
$(OUT_PREFIX)$(RUNTIME).$(O): $(RUNTIME).c precore_c.h $(CYCINC_H)
ifdef MS
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $(RUNTIME).c
else
	$(CC) -c -o $@ -include include/cyc_include.h $(CPPFLAGS) $(CFLAGS) $<
#$(CC) -pg -c -o $@ -include include/cyc_include.h $(CPPFLAGS) $(CFLAGS) $<
endif

# delete generated files with a particular prefix
clean_prefix: 
	rm -f $(addprefix $(OUT_PREFIX), $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix $(OUT_PREFIX), precore_c.h lineno.cyc)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_LIBS) $(O_LIBS) $(RUNTIME).o $(CYCLIB))
	rm -f $(addprefix *, precore_c.h lineno.cyc)
	rm -f nogc.o nogc.a
	rm -f *~ *.output *.stackdump *.d amon.out .target*
	$(MAKE) clean -C xml

# Produce dependencies
# Note we have to add cyc_include.h by hand since that only shows up in
# the C code.
$(OUT_PREFIX)%.d: %.cyc
	$(CYCDIR)/bin/$(CYCCOMP) -M -I. $< > $@

# Include the dependencies unless we are doing a clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
-include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_LIBS)))
endif
endif
