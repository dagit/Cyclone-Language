include ../Makefile.inc

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
TOVC_FLAG = -tovc
else
TOVC_FLAG =
endif

LC_FLAG=
G_FLAG=
PG_FLAG=

CYC_FLAGS += -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -save-c $(G_FLAG) $(PG_FLAG) $(LC_FLAG) $(TOVC_FLAG) $(CYC_APROF_FLAG) 

all: $(OUT_PREFIX)$(CYCLIB) nogc.a xml/libxml.a

$(OUT_PREFIX)$(CYCLIB): $(addprefix $(OUT_PREFIX), $(O_LIBS) $(RUNTIME).$(O))
ifdef MS
	lib /out:$@ $^
else
	-rm -f $@
ifeq ($(LC_FLAG),)
	ar rc $@ $^
	ranlib $@
else
	$(CYCC) $(LC_FLAG) -o $@ $^
endif
endif

nogc.a: nogc.o
	ar rc $@ $^
	ranlib $@

precore_c.h: core.h $(CYCC)
	$(CYCC) -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -o $@ -x cyc core.h

#precore_c.h: precore.h $(CYCC)
#	$(CYCC) -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -o $@ -x cyc precore.h

xml/libxml.a: 
	$(MAKE) -C xml 

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
	$(CYCC) $(CYC_FLAGS) -save-c -stopafter-toc -c -o $@ $<

ifdef MS
%.obj: %.c
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

# We need a special target because the .c file won't have the OUT_PREFIX
$(OUT_PREFIX)$(RUNTIME).$(O): $(RUNTIME).c precore_c.h $(CYCINC_H)
ifdef MS
#FIX: ignore fewer flags
	cl /I$(CYCDIR) /Dinline=__inline /c /w /nologo /Fo$@ $(RUNTIME).c
else
	$(CC) $(G_FLAG) $(PG_FLAG) -c -o $@ -include include/cyc_include.h $<
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

$(TARGET): $(addprefix $(TARGETDIR), $(C_LIBS)) $(RUNTIME).c precore_c.h nogc.c
	cp $(RUNTIME).c $(TARGETDIR)$(RUNTIME).c
	cp precore_c.h $(TARGETDIR)precore_c.h
	cp nogc.c $(TARGETDIR)nogc.c

$(TARGETDIR)%.c: %.cyc $(CYCC) $(CYCINC_H)
	$(CYCC) -D_PLATFORM_INCLUDES_="\"arch/$(TARGET).h\"" -I$(CYC_INC_PATH) -stopafter-toc $(TOVC_FLAG) -save-c -use-cpp '$(CONFIGDIR)/cyccpp $(CONFIGDIR)/arch/$(ARCH) $(CONFIGDIR)/arch/$(TARGET)' -o $@ $<
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
	$(CYCDIR)/bin/$(CYCCOMP) -M $(CYC_FLAGS) $< > $@

# Include the dependencies unless we are doing a clean
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_prefix)
-include $(addprefix $(OUT_PREFIX), $(addsuffix .d, $(CYCLONE_LIBS)))
endif
endif
