include Makefile.inc
export CYCDIR CYCC EXEC_PREFIX

CYCC=$(CYCDIR)/bin/cyclone$(EXE)
EXEC_PREFIX=$(CYCDIR)/bin/lib/cyc-lib

# Target directories:
# BB is where object files for the standard build are put
# BT is where object files for a cross build are put
# BL is where the finished library files are put
# bin is where executables are put
#
BB=build/$(build)
BT=build/$(target)
BL=bin/lib

all: directories cyclone tools

$(BL)/libcycboot.a: $(BB)/libcycboot.a
	cp $< $@

$(BL)/libcycboot_a.a: $(BB)/aprof/libcycboot.a
	cp $< $@

$(BL)/libcycboot_pg.a: $(BB)/gprof/libcycboot.a
	cp $< $@

$(BL)/libcycboot_nocheck.a: $(BB)/nocheck/libcycboot.a
	cp $< $@

$(BL)/libcyc.a: $(BB)/libcyc.a
	cp $< $@

$(BL)/libcyc_a.a: $(BB)/aprof/libcyc.a
	cp $< $@

$(BL)/libcyc_pg.a: $(BB)/gprof/libcyc.a
	cp $< $@

$(BL)/libcyc_nocheck.a: $(BB)/nocheck/libcyc.a
	cp $< $@

$(BL)/libxml.a: $(BB)/libxml.a
	cp $< $@

$(BL)/libxml_a.a: $(BB)/aprof/libxml.a
	cp $< $@

$(BL)/libxml_pg.a: $(BB)/gprof/libxml.a
	cp $< $@

$(BL)/libxml_nocheck.a: $(BB)/nocheck/libxml.a
	cp $< $@

$(BL)/cyc-lib/%/nogc.a: build/%/nogc.a
	cp $< $@

$(BL)/cyc-lib/%/nogc_a.a: build/%/aprof/nogc.a
	cp $< $@

$(BL)/cyc-lib/%/nogc_pg.a: build/%/gprof/nogc.a
	cp $< $@

$(BL)/cyc-lib/%/nogc_nocheck.a: build/%/nocheck/nogc.a
	cp $< $@

$(BL)/cyc-lib/%/runtime_cyc.a: build/%/runtime_cyc.a
	cp $< $@

$(BL)/cyc-lib/%/runtime_cyc_a.a: build/%/aprof/runtime_cyc.a
	cp $< $@

$(BL)/cyc-lib/%/runtime_cyc_pg.a: build/%/gprof/runtime_cyc.a
	cp $< $@

$(BL)/cyc-lib/%/runtime_cyc_nocheck.a: build/%/nocheck/runtime_cyc.a
	cp $< $@

bin/cyclone$(EXE): $(BB)/cyclone$(EXE)
	cp $< $@

bin/cyclone_a$(EXE): $(BB)/aprof/cyclone$(EXE)
	cp $< $@

bin/cyclone_pg$(EXE): $(BB)/gprof/cyclone$(EXE)
	cp $< $@

bin/cyclone_nocheck$(EXE): $(BB)/nocheck/cyclone$(EXE)
	cp $< $@

bin/cycdoc$(EXE): $(BB)/cycdoc$(EXE)
	cp $< $@

bin/buildlib$(EXE): $(BB)/buildlib$(EXE)
	cp $< $@

define rmake
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base $(@F)
endef

define rmake-aprof
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="-DCYC_REGION_PROFILE $(CFLAGS)" CYCFLAGS="-pa $(CYCFLAGS)" $(@F)
endef

define rmake-gprof
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="-pg $(CFLAGS)" CYCFLAGS="-pg $(CYCFLAGS)" $(@F)
endef

define rmake-nocheck
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="-DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $(CFLAGS)" CYCFLAGS="--nochecks $(CYCFLAGS)" $(@F)
endef

$(BB)/libcycboot.a:
	$(rmake)

$(BB)/aprof/libcycboot.a:
	$(rmake-aprof)

$(BB)/gprof/libcycboot.a:
	$(rmake-gprof)

$(BB)/nocheck/libcycboot.a:
	$(rmake-nocheck)

LIBCYC_PREREQS=bin/buildlib$(EXE) $(BL)/cyc-lib/$(build)/cycspecs bin/cyclone$(EXE) $(BL)/cyc-lib/cyc_include.h $(BL)/cyc-lib/$(build)/cyc_setjmp.h

# NB: as a side-effect this creates the include directory
$(BB)/libcyc.a: $(LIBCYC_PREREQS)
	$(rmake)
	for i in `(cd $(BB)/include; find * -type d)`;\
	  do mkdir -p $(BL)/cyc-lib/$(build)/include/$$i; done
	for i in `(cd $(BB)/include; find * -name '*.h')`;\
	  do cp $(BB)/include/$$i $(BL)/cyc-lib/$(build)/include/$$i; done

$(BB)/aprof/libcyc.a: $(LIBCYC_PREREQS)
	$(rmake-aprof)

$(BB)/gprof/libcyc.a: $(LIBCYC_PREREQS)
	$(rmake-gprof)

$(BB)/nocheck/libcyc.a: $(LIBCYC_PREREQS)
	$(rmake-nocheck)

$(BB)/libxml.a: tools
	$(rmake)

$(BB)/aprof/libxml.a: tools
	$(rmake-aprof)

$(BB)/gprof/libxml.a: tools
	$(rmake-gprof)

$(BB)/nocheck/libxml.a: tools
	$(rmake-nocheck)

$(BB)/nogc.a:
	$(rmake)

$(BB)/aprof/nogc.a:
	$(rmake-aprof)

$(BB)/gprof/nogc.a:
	$(rmake-gprof)

$(BB)/nocheck/nogc.a:
	$(rmake-nocheck)

$(BB)/runtime_cyc.a:
	$(rmake)

$(BB)/aprof/runtime_cyc.a:
	$(rmake-aprof)

$(BB)/gprof/runtime_cyc.a:
	$(rmake-gprof)

$(BB)/nocheck/runtime_cyc.a:
	$(rmake-nocheck)

$(BB)/cyclone$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake)

$(BB)/aprof/cyclone$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake-aprof)

$(BB)/gprof/cyclone$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake-gprof)

$(BB)/nocheck/cyclone$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake-nocheck)

$(BB)/cycdoc$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake)

$(BB)/buildlib$(EXE): $(BL)/cyc-lib/$(build)/gc.a
	$(rmake)

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

directories::
	@mkdir -p $(BL)/cyc-lib/$(build)/include

# FIX: we are not using the cycspecs file in the same way that
# gcc specs files are used, probably there should be just one
# entry for cyclone instead of separate entries for
# cyclone_target_cflags, cyclone_cc, etc.
$(BL)/cyc-lib/$(build)/cycspecs: $(CYCDIR)/config/buildspecs
	echo "*cyclone_target_cflags:" > $@
	echo "  $(CFLAGS)" >> $@
	echo "" >> $@
	echo "*cyclone_cc:" >> $@
	echo "  $(CC)" >> $@
	echo "" >> $@
	echo "*cyclone_inc_path:" >> $@
	echo "  $(INC_INSTALL)" >> $@
	echo "" >> $@
	$(CYCDIR)/config/buildspecs >> $@

$(BL)/cyc-lib/$(build)/cyc_setjmp.h: bin/cyc-lib/libc.cys bin/buildlib$(EXE)
	bin/buildlib$(EXE) -B$(BL)/cyc-lib -d $(BB)/include -setjmp > $@

$(BL)/cyc-lib/cyc_include.h: $(CYCDIR)/bin/cyc-lib/cyc_include.h
	cp $< $@

$(BL)/cyc-lib/$(build)/gc.a: gc/.libs/libgc.a
	cp -p $< $@

gc/.libs/libgc.a:
	$(MAKE) -C gc libgc.la CC="$(CC)" CFLAGS="$(CFLAGS)"

# Print version to standard output -- used by makedist
version:
	@echo $(VERSION)

tools: cyclone
	$(MAKE) -C tools/bison install
	$(MAKE) -C tools/cyclex install
	$(MAKE) -C tools/flex install
	$(MAKE) -C tools/rewrite install
	$(MAKE) -C tools/errorgen install

cyclone: \
  directories \
  $(BL)/libcycboot.a \
  $(BL)/libcyc.a \
  $(BL)/cyc-lib/$(build)/nogc.a \
  $(BL)/cyc-lib/$(build)/runtime_cyc.a \
  bin/cyclone$(EXE) \
  bin/cycdoc$(EXE) \
  bin/buildlib$(EXE)

aprof: \
  $(BL)/libcycboot_a.a \
  $(BL)/libcyc_a.a \
  $(BL)/cyc-lib/$(build)/nogc_a.a \
  $(BL)/cyc-lib/$(build)/runtime_cyc_a.a \
  bin/cyclone_a$(EXE)
	$(MAKE) -C tools/aprof install

gprof: \
  $(BL)/libcycboot_pg.a \
  $(BL)/libcyc_pg.a \
  $(BL)/cyc-lib/$(build)/nogc_pg.a \
  $(BL)/cyc-lib/$(build)/runtime_cyc_pg.a \
  bin/cyclone_pg$(EXE)

nocheck: \
  $(BL)/libcycboot_nocheck.a \
  $(BL)/cyc-lib/$(build)/nogc_nocheck.a \
  $(BL)/cyc-lib/$(build)/runtime_cyc_nocheck.a \
  bin/cyclone_nocheck$(EXE)

ifndef NO_XML_LIB
cyclone: $(BL)/libxml.a
aprof:   $(BL)/libxml_a.a
gprof:   $(BL)/libxml_pg.a
nocheck: $(BL)/libxml_nocheck.a
endif

.PHONY: all tools cyclone aprof gprof nocheck directories

########################################################################
# FOR BUILDING A CROSS COMPILER                                        #
########################################################################

# If a cross gcc is not installed you can still do some sanity
# checks on the build process by doing
#
#   make target=XXXXXX TARGET_CFLAGS=-DXXXXXX BTARGET=-DZZZZZZ all aprof gprof nocheck
#
ifneq ($(build),$(target))

$(BL)/cyc-lib/$(target)/libcycboot.a: $(BT)/libcycboot.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcycboot_a.a: $(BT)/aprof/libcycboot.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcycboot_pg.a: $(BT)/gprof/libcycboot.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcycboot_nocheck.a: $(BT)/nocheck/libcycboot.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcyc.a: $(BT)/libcyc.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcyc_a.a: $(BT)/aprof/libcyc.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcyc_pg.a: $(BT)/gprof/libcyc.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libcyc_nocheck.a: $(BT)/nocheck/libcyc.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libxml.a: $(BT)/libxml.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libxml_a.a: $(BT)/aprof/libxml.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libxml_pg.a: $(BT)/gprof/libxml.a
	cp $< $@

$(BL)/cyc-lib/$(target)/libxml_nocheck.a: $(BT)/nocheck/libxml.a
	cp $< $@

BTARGET=-b $(target)

define rmake-target
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base $(@F) CFLAGS="$(BTARGET) $(TARGET_CFLAGS)" CYCFLAGS="$(BTARGET) $(CYCFLAGS)"
endef

define rmake-target-aprof
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="$(BTARGET) -DCYC_REGION_PROFILE $(TARGET_CFLAGS)" CYCFLAGS="$(BTARGET) -pa $(CYCFLAGS)" $(@F)
endef

define rmake-target-gprof
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="$(BTARGET) -pg $(TARGET_CFLAGS)" CYCFLAGS="$(BTARGET) -pg $(CYCFLAGS)" $(@F)
endef

define rmake-target-nocheck
@mkdir -p $(@D)
make -C $(@D) -f $(CYCDIR)/Makefile_base CFLAGS="$(BTARGET) -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $(TARGET_CFLAGS)" CYCFLAGS="$(BTARGET) --nochecks $(CYCFLAGS)" $(@F)
endef

$(BT)/libcycboot.a:
	$(rmake-target)

$(BT)/aprof/libcycboot.a:
	$(rmake-target-aprof)

$(BT)/gprof/libcycboot.a:
	$(rmake-target-gprof)

$(BT)/nocheck/libcycboot.a:
	$(rmake-target-nocheck)

LIBCYC_TARGET_PREREQS=bin/buildlib$(EXE) $(BL)/cyc-lib/$(target)/cycspecs bin/cyclone$(EXE) $(BL)/cyc-lib/cyc_include.h $(BL)/cyc-lib/$(target)/cyc_setjmp.h

# NB: as a side-effect this creates the include directory
$(BT)/libcyc.a: $(LIBCYC_TARGET_PREREQS)
	$(rmake-target)
	for i in `(cd $(BT)/include; find * -type d)`;\
	  do mkdir -p $(BL)/cyc-lib/$(target)/include/$$i; done
	for i in `(cd $(BT)/include; find * -name '*.h')`;\
	  do cp $(BT)/include/$$i $(BL)/cyc-lib/$(target)/include/$$i; done

$(BT)/aprof/libcyc.a: $(LIBCYC_TARGET_PREREQS)
	$(rmake-target-aprof)

$(BT)/gprof/libcyc.a: $(LIBCYC_TARGET_PREREQS)
	$(rmake-target-gprof)

$(BT)/nocheck/libcyc.a: $(LIBCYC_TARGET_PREREQS)
	$(rmake-target-nocheck)

$(BT)/libxml.a: tools
	$(rmake-target)

$(BT)/aprof/libxml.a: tools
	$(rmake-target-aprof)

$(BT)/gprof/libxml.a: tools
	$(rmake-target-gprof)

$(BT)/nocheck/libxml.a: tools
	$(rmake-target-nocheck)

$(BT)/nogc.a:
	$(rmake-target)

$(BT)/aprof/nogc.a:
	$(rmake-target-aprof)

$(BT)/gprof/nogc.a:
	$(rmake-target-gprof)

$(BT)/nocheck/nogc.a:
	$(rmake-target-nocheck)

$(BT)/runtime_cyc.a:
	$(rmake-target)

$(BT)/aprof/runtime_cyc.a:
	$(rmake-target-aprof)

$(BT)/gprof/runtime_cyc.a:
	$(rmake-target-gprof)

$(BT)/nocheck/runtime_cyc.a:
	$(rmake-target-nocheck)

directories::
	@mkdir -p $(BL)/cyc-lib/$(target)/include

$(BL)/cyc-lib/$(target)/cycspecs: $(CYCDIR)/config/buildspecs
	echo "*cyclone_target_cflags:" > $@
	echo "  $(TARGET_CFLAGS)" >> $@
	echo "" >> $@
	echo "*cyclone_cc:" >> $@
	echo "  $(CC)" >> $@
	echo "" >> $@
	echo "*cyclone_inc_path:" >> $@
	echo "  $(INC_INSTALL)" >> $@
	echo "" >> $@
	$(CYCDIR)/config/buildspecs $(BTARGET) >> $@

$(BL)/cyc-lib/$(target)/cyc_setjmp.h: bin/cyc-lib/libc.cys bin/buildlib$(EXE)
	bin/buildlib$(EXE) $(BTARGET) -B$(BL)/cyc-lib -d $(BT)/include -setjmp > $@

# FIX: can we cross compile the gc?
cyclone: \
  $(BL)/cyc-lib/$(target)/libcycboot.a \
  $(BL)/cyc-lib/$(target)/libcyc.a \
  $(BL)/cyc-lib/$(target)/nogc.a \
  $(BL)/cyc-lib/$(target)/runtime_cyc.a

aprof: \
  $(BL)/cyc-lib/$(target)/libcycboot_a.a \
  $(BL)/cyc-lib/$(target)/libcyc_a.a \
  $(BL)/cyc-lib/$(target)/nogc_a.a \
  $(BL)/cyc-lib/$(target)/runtime_cyc_a.a

gprof: \
  $(BL)/cyc-lib/$(target)/libcycboot_pg.a \
  $(BL)/cyc-lib/$(target)/libcyc_pg.a \
  $(BL)/cyc-lib/$(target)/nogc_pg.a \
  $(BL)/cyc-lib/$(target)/runtime_cyc_pg.a

nocheck: \
  $(BL)/cyc-lib/$(target)/libcycboot_nocheck.a \
  $(BL)/cyc-lib/$(target)/libcyc_nocheck.a \
  $(BL)/cyc-lib/$(target)/nogc_nocheck.a \
  $(BL)/cyc-lib/$(target)/runtime_cyc_nocheck.a

ifndef NO_XML_LIB
cyclone: $(BL)/cyc-lib/$(target)/libxml.a
aprof:   $(BL)/cyc-lib/$(target)/libxml_a.a
gprof:   $(BL)/cyc-lib/$(target)/libxml_pg.a
nocheck: $(BL)/cyc-lib/$(target)/libxml_nocheck.a
endif

endif
#########################################################################
# END CROSS COMPILE                                                     #
#########################################################################



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
