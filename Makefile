# The following instructions assume you have not added any files to
# src or lib.  If you have, go update Makefile.inc.in in the obvious manner!

# To update to a new version of the compiler, here's the boostrap sequence
#   1. make
#   2. make cyclone_src
#   3. make update
#   4. make
# It is strongly advised that you precede step 3 with the boostrap-checking
# facilities provided by overriding CYCC, BUILDDIR, INC_PATH, and LIB_PATH.
# Also, if you want to test if the updated compiler still builds correctly,
# precede step 4 by "make clean".

# To use a new version of the compiler without nuking the C files used to
# bootstrap:
#   1. make all
#   2. make cyclone_src
# There is now a build/boot/cyclone.exe that you can use/move as you wish.

include Makefile.inc
VPATH=bin/genfiles:lib

# Target directories:
# BB is where object files for the standard build are put
# BT is where object files for a cross build are put
# BL is where the finished library files are put
# bin is where executables are put
#
BB=build/$(build)
BT=build/$(target)
BL=bin/lib

export CYCDIR CC CFLAGS CYCFLAGS

CYC_INCLUDE_H := $(BL)/cyc-lib/cyc_include.h

all: cyclone tools libs 

# Directory structure of the installed library.  (During boot,
# this is built in bin/lib ($(BL)).)
#
# lib/
#   libcyc.a                  # "Regular" lib files go here as in gcc
#   libcycboot.a              # .. maybe the boot lib isn't regular?
#   cyc-lib/                  # "Special" compiler files go here
#     cyc_include.h           # Non-arch specific
#     ARCH/                   # i686-unknown-linux, etc.
#       cyc_setjmp.h          # These are all arch specific
#       cycspecs
#       gc.a
#       include/              # Arch specific headers; non-arch spec elsewhere
#         HEADERS             # stdio.h, etc.
#       nogc.a
#       runtime_cyc.a

directories:
	-mkdir -p $(BL)/cyc-lib/$(build)/include
	-mkdir -p $(BB)   # for build temp files
ifneq ($(build),$(target))
	-mkdir -p $(BL)/cyc-lib/$(target)/include
	-mkdir -p $(BT)  # for build temp files
endif

# Print version to standard output -- used by makedist
version:
	@echo $(VERSION)

cyclone: \
	directories \
	$(CYC_INCLUDE_H) \
	$(BL)/cyc-lib/$(build)/gc.a \
	$(BL)/$(CYCBOOTLIB) \
	$(addprefix bin/, cyclone$(EXE) cycdoc$(EXE) buildlib$(EXE)) \
	$(BL)/cyc-lib/$(build)/cycspecs \
	$(addprefix $(BL)/cyc-lib/$(build)/, nogc.a runtime_cyc.a) \
	$(BL)/cyc-lib/$(build)/cyc_setjmp.h \
	$(BL)/$(CYCLIB)

aprof: \
  $(BL)/libcycboot_a.a \
  $(BL)/libcyc_a.a \
  $(addprefix $(BL)/cyc-lib/$(build)/, nogc_a.a runtime_cyc_a.a) \
  a_libs \
  bin/cyclone_a$(EXE)
	$(MAKE) -C tools/aprof install

gprof: $(BL)/libcyc_pg.a \
  $(addprefix $(BL)/cyc-lib/$(build)/, nogc_pg.a runtime_cyc_pg.a) \
  bin/cyclone_pg$(EXE)

nocheck: $(BL)/libcyc_nocheck.a \
  $(addprefix $(BL)/cyc-lib/$(build)/, nogc.a runtime_cyc.a)

#vvvvvvvvvvvvvvvvvvvvvvv# FOR CROSS COMPILE #vvvvvvvvvvvvvvvvvvvvvvv#
# FIX: can we cross-compile gc?
# FIX: cross-compile libxml.a
ifneq ($(build),$(target))
cyclone: \
	$(BL)/cyc-lib/$(target)/cycspecs \
	$(addprefix $(BL)/cyc-lib/$(target)/, nogc.a runtime_cyc.a) \
	$(BL)/cyc-lib/$(target)/cyc_setjmp.h \
	$(BL)/cyc-lib/$(target)/$(CYCLIB)

aprof: \
  $(BL)/cyc-lib/$(target)/libcycboot_a.a \
  $(BL)/cyc-lib/$(target)/libcyc_a.a \
  $(addprefix $(BL)/cyc-lib/$(target)/, nogc_a.a runtime_cyc_a.a)

gprof: $(BL)/cyc-lib/$(target)/libcyc_pg.a \
  $(addprefix $(BL)/cyc-lib/$(target)/, nogc_pg.a runtime_cyc_pg.a)

nocheck: $(BL)/cyc-lib/$(target)/libcyc_nocheck.a \
  $(addprefix $(BL)/cyc-lib/$(target)/, nogc.a runtime_cyc.a)
endif
#^^^^^^^^^^^^^^^^^^^^^^^# END CROSS COMPILE #^^^^^^^^^^^^^^^^^^^^^^^#

tools:
	$(MAKE) -C tools/bison  install 
	$(MAKE) -C tools/cyclex install 
	$(MAKE) -C tools/flex   install 
	$(MAKE) -C tools/rewrite install
	$(MAKE) -C tools/errorgen install

ifndef NO_XML_LIB
libs:
	$(MAKE) -C lib/xml install 
a_libs:
	$(MAKE) -C lib/xml a_install
else
libs:
a_libs:
endif

.PHONY: all tools cyclone aprof gprof libs nocheck directories

############################ EXECUTABLES ############################
bin/cyclone$(EXE): \
  $(addprefix $(BB)/, $(O_SRCS) install_path.$(O)) \
  $(BL)/$(CYCBOOTLIB) \
  $(BL)/cyc-lib/$(build)/runtime_cyc.a \
  $(BL)/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/cyclone_a$(EXE): \
  $(addprefix $(BB)/, $(A_SRCS) install_path.$(O)) \
  $(BL)/libcycboot_a.a \
  $(BL)/cyc-lib/$(build)/runtime_cyc_a.a \
  $(BL)/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/cyclone_pg$(EXE): \
  $(addprefix $(BB)/, $(PG_SRCS) install_path.$(O)) \
  $(BL)/$(CYCBOOTLIB) \
  $(BL)/cyc-lib/$(build)/runtime_cyc_pg.a \
  $(BL)/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)

bin/cycdoc$(EXE): \
  $(addprefix $(BB)/, $(addsuffix .$(O), $(CYCDOC_SRCS)) install_path.$(O)) \
  $(BL)/$(CYCBOOTLIB) \
  $(BL)/cyc-lib/$(build)/runtime_cyc.a \
  $(BL)/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/buildlib$(EXE): \
  $(addprefix $(BB)/, $(addsuffix .$(O), $(BUILDLIB_SRCS)) install_path.$(O))\
  $(BL)/$(CYCBOOTLIB) \
  $(BL)/cyc-lib/$(build)/runtime_cyc.a \
  $(BL)/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

########################### SPECIAL FILES ###########################

$(CYC_INCLUDE_H): $(CYCDIR)/bin/cyc-lib/cyc_include.h
	cp $< $@

# NB: install_path.c is not needed for the cross compile case
# since we don't cross compile the compiler itself
# FIX: the wchar stuff should be moved to the cycspecs file
$(BB)/install_path.c: $(CYCDIR)/Makefile.inc
	 (echo "char *Carch = \"$(build)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Cversion = \"$(VERSION)\";"; \
	  echo "int Wchar_t_unsigned = $(WCHAR_T_UNSIGNED);"; \
	  echo "int Sizeof_wchar_t = $(SIZEOF_WCHAR_T);") > $@

# Build the cycspecs file.
# FIX: we are not using the specs file in the same way that
# gcc spec files are used, probably there should be just one
# entry for cyclone instead of separate entries for
# cyclone_target_cflags, cyclone_cc, etc.
$(BL)/cyc-lib/$(build)/cycspecs:
	echo "*cyclone_target_cflags:" > $@
	echo "  $(CFLAGS)" >> $@
	echo "" >> $@
	echo "*cyclone_cc:" >> $@
	echo "  $(CC)" >> $@
	echo "" >> $@
	echo "*cyclone_inc_path:" >> $@
	echo "  $(INC_INSTALL)" >> $@
	echo "" >> $@
	${CYCDIR}/config/buildspecs >> $@

# The buildlib process creates many files; exactly what files depends
# on the architecture.  So, we give just three targets: cstubs.c,
# cycstubs.cyc, and cyc_setjmp.h, which are always made by buildlib no
# matter what.  Other files are silently created by side effect.
$(BL)/cyc-lib/$(build)/cyc_setjmp.h: bin/cyc-lib/libc.cys
	bin/buildlib -B$(BL)/cyc-lib -d $(BB)/include -setjmp > $@

$(BB)/include/cstubs.c: $(BB)/include/cycstubs.cyc

# FIX: the rule creates a bunch of header files but the header
# files aren't targets, so, dependencies are wrong and make can
# get confused.
$(BB)/include/cycstubs.cyc: bin/cyc-lib/libc.cys
	bin/buildlib -B$(BL)/cyc-lib -d $(@D) $<
#	cp -r $(@D)/* $(BL)/cyc-lib/$(build)/include
	for i in `(cd $(BB)/include; find * -type d)`;\
	  do mkdir -p $(BL)/cyc-lib/$(build)/include/$$i; done
	for i in `(cd $(BB)/include; find * -name '*.h')`;\
	  do cp $(BB)/include/$$i $(BL)/cyc-lib/$(build)/include/$$i; done

$(BB)/include/precore_c.h: \
  include/core.h \
  bin/cyclone$(EXE) \
  $(BL)/cyc-lib/$(build)/cycspecs
	bin/cyclone$(EXE) -Iinclude -B$(BL)/cyc-lib -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -nocyc_setjmp -x cyc -o $@ $<

#vvvvvvvvvvvvvvvvvvvvvvv# FOR CROSS COMPILE #vvvvvvvvvvvvvvvvvvvvvvv#
ifneq ($(build),$(target))
$(BL)/cyc-lib/$(target)/cycspecs:
	echo "*cyclone_target_cflags:" > $@
	echo "  $(TARGET_CFLAGS)" >> $@
	echo "" >> $@
	echo "*cyclone_cc:" >> $@
	echo "  $(CC)" >> $@
	echo "" >> $@
	echo "*cyclone_inc_path:" >> $@
	echo "  $(INC_INSTALL)" >> $@
	echo "" >> $@
	${CYCDIR}/config/buildspecs $(BTARGET) >> $@

$(BL)/cyc-lib/$(target)/cyc_setjmp.h: bin/cyc-lib/libc.cys
	bin/buildlib $(BTARGET) -B$(BL)/cyc-lib -d $(BT)/include -setjmp > $@

$(BT)/include/cstubs.c: $(BT)/include/cycstubs.cyc

$(BT)/include/cycstubs.cyc: bin/cyc-lib/libc.cys
	bin/buildlib $(BTARGET) -B$(BL)/cyc-lib -d $(@D) $<
#	cp -r $(@D)/* $(BL)/cyc-lib/$(target)/include
	for i in `(cd $(BT)/include; find * -type d)`;\
	  do mkdir -p $(BL)/cyc-lib/$(target)/include/$$i; done
	for i in `(cd $(BT)/include; find * -name '*.h')`;\
	  do cp $(BT)/include/$$i $(BL)/cyc-lib/$(target)/include/$$i; done

$(BT)/include/precore_c.h: \
  include/core.h \
  bin/cyclone$(EXE) \
  $(BL)/cyc-lib/$(target)/cycspecs
	bin/cyclone$(EXE) $(BTARGET) -Iinclude -B$(BL)/cyc-lib -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -nocyc_setjmp -x cyc -o $@ $<
endif
#^^^^^^^^^^^^^^^^^^^^^^^# END CROSS COMPILE #^^^^^^^^^^^^^^^^^^^^^^^#

############################# LIBRARIES #############################
$(BL)/cyc-lib/$(build)/gc.a: gc/.libs/libgc.a
	cp -p $< $@

gc/.libs/libgc.a:
	$(MAKE) -C gc libgc.la CC="$(CC)" CFLAGS="$(CFLAGS)"

# gc/gc.a:
#	$(MAKE) -C gc CC="$(CC)" gc.a CFLAGS="$(CFLAGS) -O -I./include -DATOMIC_UNCOLLECTABLE -DNO_SIGNALS -DNO_EXECUTE_PERMISSION -DALL_INTERIOR_POINTERS -DNO_DEBUGGING -DDONT_ADD_BYTE_AT_END"

$(BL)/$(CYCBOOTLIB): \
  $(addprefix $(BB)/, $(O_BOOT_LIBS)) \
  $(BB)/boot_cstubs.$(O) \
  $(BB)/boot_cycstubs.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/libcycboot_a.a: \
  $(addprefix $(BB)/, $(A_BOOT_LIBS)) \
  $(BB)/boot_cstubs_a.$(O) \
  $(BB)/boot_cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

# NB: the invocation
#    $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG))
# expands to $(OPTFLAG) if the target is not in $(O_NOOPT_SRCS),
# and to the empty string otherwise.  We use it to turn off optimization
# for files where it is expensive (parse_tab.o).
#
$(BB)/%.$(O): %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) $< -o $@

$(BB)/%.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

# This rule for install_path
%_a.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DCYC_REGION_PROFILE $<

# This rule for the rest
$(BB)/%_a.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DCYC_REGION_PROFILE $<

$(BB)/%_a.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -pa -c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

%_pg.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -pg $<

$(BB)/%_pg.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -pg $<

$(BB)/%_pg.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -pg -c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

%_nocheck.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $<

$(BB)/%_nocheck.$(O): %.c
	$(CC) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $<

$(BB)/%_nocheck.$(O): %.cyc cyclone
	bin/cyclone$(EXE) --nochecks -c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

$(BL)/$(CYCLIB): $(BL)/cyc-lib/$(build)/cyc_setjmp.h
$(BL)/$(CYCLIB): \
  $(BB)/include/cstubs.$(O) \
  $(BB)/include/cycstubs.$(O) \
  $(addprefix $(BB)/, $(O_BOOT_LIBS)) \
  $(addprefix $(BB)/, $(O_LIBS))
	-$(RM) $@
	ar rc $@ \
	  $(addprefix $(BB)/, $(O_LIBS)) \
	  $(addprefix $(BB)/, $(O_BOOT_LIBS)) \
	  $(BB)/include/cstubs.$(O) \
	  $(BB)/include/cycstubs.$(O)
	$(RANLIB) $@

$(BL)/libcyc_a.a: \
  $(addprefix $(BB)/, $(A_BOOT_LIBS) $(A_LIBS)) \
  $(addprefix $(BB)/include/, cstubs_a.$(O) cycstubs_a.$(O))
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/libcyc_pg.a: \
  $(addprefix $(BB)/, $(PG_BOOT_LIBS)) \
  $(addprefix $(BB)/, $(PG_LIBS)) \
  $(BB)/include/cstubs_pg.$(O) \
  $(BB)/include/cycstubs_pg.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/libcyc_nocheck.a: \
  $(addprefix $(BB)/, $(NOCHECK_BOOT_LIBS)) \
  $(addprefix $(BB)/, $(NOCHECK_LIBS)) \
  $(BB)/include/cstubs_nocheck.$(O) \
  $(BB)/include/cycstubs_nocheck.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/cyc-lib/%/nogc.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/%/nogc.a: build/%/nogc.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(build)/nogc_a.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/$(build)/nogc_a.a: \
  $(BB)/nogc_a.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(build)/nogc_pg.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/$(build)/nogc_pg.a: \
  $(BB)/nogc_pg.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(build)/runtime_cyc.a: \
  $(addprefix $(BB)/, $(O_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BB)/, $(O_RUNTIME))
	$(RANLIB) $@

$(BL)/cyc-lib/$(build)/runtime_cyc_a.a: \
  $(addprefix $(BB)/, $(A_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BB)/, $(A_RUNTIME))
	$(RANLIB) $@

$(BL)/cyc-lib/$(build)/runtime_cyc_pg.a: \
  $(addprefix $(BB)/, $(PG_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BB)/, $(PG_RUNTIME))
	$(RANLIB) $@

$(BB)/include/cstubs.$(O): $(BB)/include/cstubs.c $(BB)/include/precore_c.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BB)/include/cycstubs.$(O): $(BB)/include/cycstubs.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -save-c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib -c -o $@ $<

#vvvvvvvvvvvvvvvvvvvvvvv# FOR CROSS COMPILE #vvvvvvvvvvvvvvvvvvvvvvv#
ifneq ($(build),$(target))
$(BL)/cyc-lib/$(target)/libcycboot_a.a: \
  $(addprefix $(BT)/, $(A_BOOT_LIBS)) \
  $(BT)/boot_cstubs_a.$(O) \
  $(BT)/boot_cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BT)/%.o: %.c
	$(CC) $(BTARGET) -c $(CFLAGS) $(CPPFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) $< -o $@

$(BT)/%.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -c -Iinclude -I$(BL)/cyc-lib/$(target)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

$(BT)/%_a.$(O): %.c
	$(CC) $(BTARGET) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DCYC_REGION_PROFILE $<

$(BT)/%_a.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -pa -c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

$(BT)/%_pg.$(O): %.c
	$(CC) $(BTARGET) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -pg $<

$(BT)/%_pg.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -pg -c -Iinclude -I$(BL)/cyc-lib/$(target)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

$(BT)/%_nocheck.$(O): %.c
	$(CC) $(BTARGET) $(CFLAGS) $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $<

$(BT)/%_nocheck.$(O): %.cyc cyclone
	bin/cyclone$(EXE) $(BTARGET) --nochecks -c -Iinclude -I$(BL)/cyc-lib/$(target)/include -B$(BL)/cyc-lib $(if $(findstring $(notdir $@), $(O_NOOPT_SRCS)), , $(OPTFLAG)) -o $@ $(CYCFLAGS) $<

$(BL)/cyc-lib/$(target)/$(CYCLIB): $(BL)/cyc-lib/$(target)/cyc_setjmp.h
$(BL)/cyc-lib/$(target)/$(CYCLIB): \
  $(BT)/include/cstubs.$(O) \
  $(BT)/include/cycstubs.$(O) \
  $(addprefix $(BT)/, $(O_LIBS)) \
  $(addprefix $(BT)/, $(O_BOOT_LIBS))
	-$(RM) $@
	ar rc $@ \
	  $(addprefix $(BT)/, $(O_LIBS)) \
	  $(addprefix $(BT)/, $(O_BOOT_LIBS)) \
	  $(BT)/include/cstubs.$(O) \
	  $(BT)/include/cycstubs.$(O)
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/libcyc_a.a: \
  $(addprefix $(BT)/, $(A_BOOT_LIBS) $(A_LIBS)) \
  $(addprefix $(BT)/include/, cstubs_a.$(O) cycstubs_a.$(O))
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/libcyc_pg.a: \
  $(addprefix $(BT)/, $(PG_BOOT_LIBS)) \
  $(addprefix $(BT)/, $(PG_LIBS)) \
  $(BT)/include/cstubs_pg.$(O) \
  $(BT)/include/cycstubs_pg.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/libcyc_nocheck.a: \
  $(addprefix $(BT)/, $(NOCHECK_BOOT_LIBS)) \
  $(addprefix $(BT)/, $(NOCHECK_LIBS)) \
  $(BT)/include/cstubs_nocheck.$(O) \
  $(BT)/include/cycstubs_nocheck.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/nogc.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/$(target)/nogc.a: $(BT)/nogc.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/nogc_a.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/$(target)/nogc_a.a: \
  $(BT)/nogc_a.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/nogc_pg.a: bin/genfiles/malloc.c
$(BL)/cyc-lib/$(target)/nogc_pg.a: \
  $(BT)/nogc_pg.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/runtime_cyc.a: \
  $(addprefix $(BT)/, $(O_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BT)/, $(O_RUNTIME))
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/runtime_cyc_a.a: \
  $(addprefix $(BT)/, $(A_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BB)/, $(A_RUNTIME))
	$(RANLIB) $@

$(BL)/cyc-lib/$(target)/runtime_cyc_pg.a: \
  $(addprefix $(BT)/, $(PG_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix $(BT)/, $(PG_RUNTIME))
	$(RANLIB) $@

$(BT)/include/cstubs.$(O): $(BT)/include/cstubs.c $(BT)/include/precore_c.h
	$(CC) $(BTARGET) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BT)/include/cycstubs.$(O): $(BT)/include/cycstubs.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -save-c -Iinclude -I$(BL)/cyc-lib/$(build)/include -B$(BL)/cyc-lib -c -o $@ $<
endif
#^^^^^^^^^^^^^^^^^^^^^^^# END CROSS COMPILE #^^^^^^^^^^^^^^^^^^^^^^^#

# Store the compiler, libraries, and tools in the user-defined directories.
# Also, keep a record of what was copied for later uninstall.
install: all inc_install lib_install bin_install
uninstall: inc_uninstall lib_uninstall bin_uninstall

ifdef INC_INSTALL
inc_install:
	$(SHELL) config/cyc_install include/* $(INC_INSTALL)
inc_uninstall:
	$(SHELL) config/cyc_install -u $(INC_INSTALL)
else
inc_install inc_uninstall:
	@(echo "no include directory specified"; exit 1)
endif
ifdef BIN_INSTALL
bin_install:
	$(SHELL) config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) bin/cycflex$(EXE) bin/rewrite$(EXE) bin/aprof$(EXE) bin/errorgen$(EXE) $(BIN_INSTALL)
bin_uninstall:
	$(SHELL) config/cyc_install -u $(BIN_INSTALL)
else
bin_install bin_uninstall:
	@(echo "no bin directory specified"; exit 1)
endif
ifdef LIB_INSTALL
lib_install:
	$(SHELL) config/cyc_install $(BL)/* $(LIB_INSTALL)
lib_uninstall:
	$(SHELL) config/cyc_install -u $(LIB_INSTALL)
else
lib_install lib_uninstall:
	@(echo "no lib directory specified"; exit 1)
endif
.PHONY: install uninstall inc_install lib_install bin_install
.PHONY: inc_uninstall lib_uninstall bin_uninstall unupdate

BUILDDIR=build/boot
GENDIR=bin/genfiles

# These build off the Cyclone source files, but do not replace anything in bin
# We override BUILDDIR and CYCFLAGS for many nefarious purposes:
#   cross-compiling, bootstrapping, profiling, ...
# We use the -r flag to eliminate built-in make rules.  This is needed
#   because of a circular dependence that arises when bootstrapping as with
#   make cyclone_src BUILDDIR=build/boot1 CYCC=`pwd`/build/boot/cyclone
ifdef DEBUGBUILD
LIBSRC_FLAGS += DEBUGBUILD=X
endif
ifdef PROFILE
LIBSRC_FLAGS += PROFILE=X
endif
ifdef ALLOC_PROFILE
LIBSRC_FLAGS += ALLOC_PROFILE=X
endif
ifdef NOGC
LIBSRC_FLAGS += NOGC=X
endif
ifdef NOREGIONS
LIBSRC_FLAGS += NOREGIONS=X
endif
ifdef OPTBUILD
LIBSRC_FLAGS += OPTBUILD=X
endif
DO_LIBSRC=$(MAKE) -r -C $(BUILDDIR) -f $(CYCDIR)/Makefile_libsrc 

cyclone_src:
	-mkdir -p $(BUILDDIR)
	$(DO_LIBSRC) all
lib_src:
	-mkdir -p $(BUILDDIR)
	$(DO_LIBSRC) libs
cfiles:
	-mkdir -p $(BUILDDIR)
	$(DO_LIBSRC) cfiles

# Note: Tried doing this stuff with target-specific variables instead
#       of recursive invocations, but it crashes gnumake 3.79.1
#       due to "export CYCFLAGS" and "dbg_src: override CYCFLAGS+=-g".
#       They say it's been fixed in their working version.  The crash
#       happens only when the rule (e.g. dbg_src) fires.
dbg_src:
	$(MAKE) BUILDDIR=build/dbg CYCFLAGS="$(CYCFLAGS) -g" cyclone_src
dbg_lib_src:
	$(MAKE) BUILDDIR=build/dbg CYCFLAGS="$(CYCFLAGS) -g" lib_src

# The directory in which the "unupdate" information is stored.
UNUPDATEDIR=unupdate

# The update files that go from BUILDDIR to GENDIR.
BUILDDIR_UPDATE_FILES=$(UPDATE_SRCS) $(C_BOOT_LIBS) precore_c.h boot_cycstubs.c

# The update files that go from "lib" to GENDIR.
LIB_UPDATE_FILES=boot_cstubs.c nogc.c malloc.c $(C_RUNTIME)

# The update files that go from "lib" to "bin/cyc-lib".
CYC_LIB_UPDATE_FILES=cyc_include.h libc.cys

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
XS=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
cmp: 
	@for i in $(BUILDDIR_UPDATE_FILES);\
	  do (cmp -s $(GENDIR)/$$i $(BUILDDIR)/$$i\
	      || echo $(XS) $$i CHANGED) done
	@for i in $(CYCLONE_H);\
	  do (test ! -e lib/$$i || cmp -s include/$$i lib/$$i\
              || echo $(XS) lib/$$i CHANGED) done
	@for i in $(LIB_UPDATE_FILES);\
          do (cmp -s $(GENDIR)/$$i lib/$$i\
              || echo $(XS) $$i CHANGED) done
	@for i in $(CYC_LIB_UPDATE_FILES);\
          do (test ! -e lib/$$i\
	      || cmp -s bin/cyc-lib/$$i lib/$$i\
              || echo $(XS) $$i CHANGED) done

# This target updates what is in bin/genfiles and include.
# It would be "dangerous" to invoke this target if we did not have 
# version control.  Only updates changed files (makes cvs faster).
# A backup copy is stored in UNUPDATEDIR; it can be restored by
# issuing "make unupdate". Only the latest "update" is kept.
update: cfiles
	@test ! -e $(UNUPDATEDIR) || mv $(UNUPDATEDIR) $(UNUPDATEDIR)-backup
	@mkdir -p $(UNUPDATEDIR)/include $(UNUPDATEDIR)/genfiles $(UNUPDATEDIR)/lib $(UNUPDATEDIR)/bin/cyc-lib
	@for i in $(BUILDDIR_UPDATE_FILES);\
           do (cmp -s $(BUILDDIR)/$$i $(GENDIR)/$$i\
               || (echo UPDATING $(GENDIR)/$$i;\
		   cp -pR $(GENDIR)/$$i $(UNUPDATEDIR)/genfiles/$$i;\
		   touch $(UNUPDATEDIR)/contains-files;\
	           cp $(BUILDDIR)/$$i $(GENDIR)/$$i)) done
	@for i in $(LIB_UPDATE_FILES);\
           do (cmp -s lib/$$i $(GENDIR)/$$i\
               || (echo UPDATING $(GENDIR)/$$i;\
		   cp -pR $(GENDIR)/$$i $(UNUPDATEDIR)/genfiles/$$i;\
		   touch $(UNUPDATEDIR)/contains-files;\
                   cp lib/$$i $(GENDIR)/$$i)) done
	@for i in $(CYCLONE_H);\
           do (test ! -e lib/$$i || cmp -s lib/$$i include/$$i\
               || (echo UPDATING include/$$i;\
		   cp -pR include/$$i $(UNUPDATEDIR)/include/$$i;\
		   cp -pR lib/$$i $(UNUPDATEDIR)/lib/$$i;\
		   touch $(UNUPDATEDIR)/contains-files;\
                   mv lib/$$i include/$$i)) done
	@for i in $(CYC_LIB_UPDATE_FILES);\
           do (test ! -e lib/$$i\
               || cmp -s lib/$$i bin/cyc-lib/$$i\
               || (echo UPDATING bin/cyc-lib/$$i;\
		   cp -pR bin/cyc-lib/$$i $(UNUPDATEDIR)/bin/cyc-lib/$$i;\
		   cp -pR lib/$$i $(UNUPDATEDIR)/lib/$$i;\
		   touch $(UNUPDATEDIR)/contains-files;\
                   mv lib/$$i bin/cyc-lib/$$i)) done
	@test -e $(UNUPDATEDIR)/contains-files || \
           ($(RM) -r $(UNUPDATEDIR) && mv $(UNUPDATEDIR)-backup $(UNUPDATEDIR) \
            && echo "No changes found; keeping unupdate information from previous update.")


# Undo the most recent update. This can only be done once after an update; if the unupdate is
# successful, the unupdate directory is removed. Because it is very risky to keep the compiler
# built by the previous update around (this compiler would be used to build the C files for
# the next update), a clean bootstrap is executed immediately after restoring the old files.
unupdate:
	@test -e $(UNUPDATEDIR) || (echo "There is nothing to unupdate." && false)
	@test ! -e $(UNUPDATEDIR)/genfiles/* || cp -av $(UNUPDATEDIR)/genfiles/* $(GENDIR)
	@test ! -e $(UNUPDATEDIR)/lib/* || cp -pR $(UNUPDATEDIR)/lib/* lib
	@test ! -e $(UNUPDATEDIR)/include/* || cp -pR $(UNUPDATEDIR)/include/* include
	@test ! -e $(UNUPDATEDIR)/bin/cyc-lib/* || cp -pR $(UNUPDATEDIR)/bin/cyc-lib/* bin/cyc-lib
	@$(RM) -r $(UNUPDATEDIR)
	@echo
	@echo "Unupdate complete. Now \"make clean\" and \"make all\" will be run to restore your"
	@echo "compiler, tools and libraries to their previous state."
	@echo
	$(MAKE) clean
	$(MAKE) all
	@echo
	@echo "Unupdate is complete."
	@echo

# Test the bootstrap sequence and execute the test suite on the bootstrapped compiler;
# if this fails, unupdate 
safeupdate: test_src_compiler
	@($(MAKE) update && $(MAKE) clean && $(MAKE) all && $(MAKE) test) \
           || (echo && echo "XXXXXXXXX Safe update failed; unupdating:" && echo\
               && $(MAKE) unupdate \
               && echo && echo "XXXXXXXXX Safe update failed." && echo && false)
	@echo
	@echo "The test suite did not return with an error, so the update has been retained."
	@echo "However, be sure to check the output of the testsuite for any problems, as"
	@echo "the testsuite may ignore the errors it finds. If you see any problems, you can"
	@echo "undo this update by running \"make unupdate\" until the next time you do a"
	@echo "\"make update\" or \"make safeupdate\"."
	@echo

test_src_compiler: cyclone_src
	@$(RM) -r build/boot_test_src_compiler
	$(MAKE) cyclone_src BUILDDIR=build/boot_test_src_compiler CYCC=`pwd`/$(BUILDDIR)/cyclone
	@cmp -s build/boot_test_src_compiler/cyclone `pwd`/$(BUILDDIR)/cyclone || \
            (echo "XXXXXXXXX Compiler cannot build itself correctly." && false)

# a little testing (much more is in the tests directory)
test:
	$(MAKE) -C tests CYCFLAGS="-g -save-c -pp"
test_bin:
	$(MAKE) -C tests\
         CYCC=$(CYCDIR)/bin/cyclone$(EXE)\
         CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)\
         CYCFLAGS="-L$(CYCDIR)/$(BL) -g -save-c -pp -I$(CYCDIR)/include -B$(CYCDIR)/$(BL)/cyc-lib"
# The -I and -B flags are explained in Makefile_libsrc and should be
# kept in sync with the settings there
test_boot:
	$(MAKE) -C tests\
         CYCC=$(CYCDIR)/build/boot/cyclone$(EXE)\
         CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)\
         CYCFLAGS="$(addprefix -I$(CYCDIR)/build/boot/, . include) $(addprefix -I$(CYCDIR)/, lib src include) -B$(CYCDIR)/build/boot -B$(CYCDIR)/lib -g -save-c -pp"\
	 LDFLAGS="-L$(CYCDIR)/build/boot -B$(CYCDIR)/$(BL)/cyc-lib -v"

clean_test:
	$(MAKE) -C tests clean

# To do: a much safer and less kludgy way to clean build!
# To do: a way to clean individual directories in build.
clean_build:
	mv build/CVS .build_CVS; \
	EXITC=$$?; \
	$(RM) -rf build/*; \
	if [ "$$EXITC" = 0 ]; then \
	mv .build_CVS build/CVS; \
	fi

clean_nogc: clean_test clean_build
	$(MAKE) -C tools/bison  clean
	$(MAKE) -C tools/cyclex clean
	$(MAKE) -C tools/flex   clean
	$(MAKE) -C tools/rewrite clean
	$(MAKE) -C tools/aprof  clean
	$(MAKE) -C tools/errorgen clean
	$(MAKE) -C tests        clean
#	$(MAKE) -C doc          clean
	$(MAKE) -C lib/xml      clean
	$(RM) -r $(BL)
	$(RM) $(addprefix bin/, $(addsuffix $(EXE), cyclone cyclone_a cycdoc buildlib cycbison cyclex cycflex aprof rewrite errorgen))
	$(RM) *~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
