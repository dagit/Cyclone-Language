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

export CYCDIR CC CFLAGS CYCFLAGS

CYC_INCLUDE_H := bin/lib/cyc-lib/cyc_include.h

all: cyclone tools libs 

# Print version to standard output -- used by makedist
version:
	@echo $(VERSION)

cyclone: \
	directory-structure \
	$(CYC_INCLUDE_H) \
	bin/lib/cyc-lib/$(build)/gc.a \
	bin/lib/$(CYCBOOTLIB) \
	$(addprefix bin/, cyclone$(EXE) cycdoc$(EXE) buildlib$(EXE)) \
	bin/lib/cyc-lib/$(build)/cycspecs \
	$(addprefix bin/lib/cyc-lib/$(build)/, nogc.a runtime_cyc.a) \
	bin/lib/cyc-lib/$(build)/cyc_setjmp.h \
	bin/lib/$(CYCLIB)

# FIX: can we cross-compile gc?
# FIX: cross-compile libxml.a
# FIX: cross-compile the _a, _pg, _nocheck variants
ifneq ($(build),$(target))
cyclone: \
	bin/lib/cyc-lib/$(target)/cycspecs \
	$(addprefix bin/lib/cyc-lib/$(target)/, nogc.a runtime_cyc.a) \
	bin/lib/cyc-lib/$(target)/cyc_setjmp.h \
	bin/lib/cyc-lib/$(target)/$(CYCLIB)
endif

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

aprof: \
  bin/lib/libcycboot_a.a \
  bin/lib/libcyc_a.a \
  $(addprefix bin/lib/cyc-lib/$(build)/, nogc_a.a runtime_cyc_a.a) \
  a_libs
	$(MAKE) -C tools/aprof install

cyclone_a: aprof bin/cyclone_a$(EXE)

cyclone_pg: bin/cyclone_pg$(EXE)

gprof: bin/lib/libcyc_pg.a \
  $(addprefix bin/lib/cyc-lib/$(build)/, nogc_pg.a runtime_cyc_pg.a)

nocheck: bin/lib/libcyc_nocheck.a \
  $(addprefix bin/lib/cyc-lib/$(build)/, nogc.a runtime_cyc.a)

.PHONY: all tools cyclone aprof gprof libs nocheck directory-structure

# Executables for the bin directory
bin/cyclone$(EXE): \
  $(addprefix build/$(build)/, $(O_SRCS) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(build)/runtime_cyc.a \
  bin/lib/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/cyclone_a$(EXE): \
  $(addprefix bin/genfiles/, $(A_SRCS) install_path.$(O)) \
  bin/lib/libcycboot_a.a \
  bin/lib/cyc-lib/$(build)/runtime_cyc_a.a \
  bin/lib/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/cyclone_pg$(EXE): \
  $(addprefix bin/genfiles/, $(PG_SRCS) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(build)/runtime_cyc_pg.a \
  bin/lib/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -pg -o $@ $^ $(LDFLAGS)

bin/cycdoc$(EXE): \
  $(addprefix build/$(build)/, $(addsuffix .$(O), $(CYCDOC_SRCS)) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(build)/runtime_cyc.a \
  bin/lib/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/buildlib$(EXE): \
  $(addprefix build/$(build)/, $(addsuffix .$(O), $(BUILDLIB_SRCS)) install_path.$(O))\
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(build)/runtime_cyc.a \
  bin/lib/cyc-lib/$(build)/gc.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/lib/$(CYCBOOTLIB): \
  $(addprefix build/$(build)/, $(O_BOOT_LIBS)) \
  build/$(build)/boot_cstubs.$(O) \
  build/$(build)/boot_cycstubs.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

bin/lib/libcycboot_a.a: \
  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
  bin/genfiles/boot_cstubs_a.$(O) \
  bin/genfiles/boot_cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

# Have two sets of rules below, to differentiate which files require
# optimization and which do not.  If the NOOPT flag is set, we just
# compile the file.  Otherwise, we compile it via a recursive call with
# NOOPT off, setting optimization flags as needed.
NOOPT=
ifdef NOOPT
build/$(build)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

ifneq ($(build),$(target))
build/$(target)/%.o: %.c
	$(CC) $(BTARGET) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
endif

build/$(build)/%.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -o $@ $(CYCFLAGS) $<

ifneq ($(build),$(target))
build/$(target)/%.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -c -Iinclude -Ibin/lib/cyc-lib/$(target)/include -Bbin/lib/cyc-lib -o $@ $(CYCFLAGS) $<
endif

%_a.$(O): %.c
	$(CC) $(CFLAGS) -c -o $@ -DCYC_REGION_PROFILE $<

%_a.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -pa -c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -o $@ $(CYCFLAGS) $<

%_pg.$(O): %.c
	$(CC) $(CFLAGS) -c -o $@ -pg $<

%_pg.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -pg -c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -o $@ $(CYCFLAGS) $<

%_nocheck.$(O): %.c
	$(CC) $(CFLAGS) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $<

%_nocheck.$(O): %.cyc cyclone
	bin/cyclone$(EXE) --nochecks -c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -o $@ $(CYCFLAGS) $<
else
build/$(build)/%.$(O): %.c
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CFLAGS=$(CFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

ifneq ($(build),$(target))
build/$(target)/%.$(O): %.c
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CFLAGS=$(CFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)
endif

build/$(build)/%.$(O): %.cyc bin/cyclone$(EXE)
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CYCFLAGS=$(CYCFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

ifneq ($(build),$(target))
build/$(target)/%.$(O): %.cyc bin/cyclone$(EXE)
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CYCFLAGS=$(CYCFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)
endif

%_a.$(O): %.c
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CFLAGS=$(CFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

%_a.$(O): %.cyc bin/cyclone$(EXE)
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CYCFLAGS=$(CYCFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

%_pg.$(O): %.c
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CFLAGS=$(CFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

%_pg.$(O): %.cyc bin/cyclone$(EXE)
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CYCFLAGS=$(CYCFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

%_nocheck.$(O): %.c
	@(if [ -z "$(findstring $(notdir $@),$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CFLAGS=$(CFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)

%_nocheck.$(O): %.cyc bin/cyclone$(EXE)
	@(if [ -z "$(findstring $@,$(O_NOOPT_SRCS))" ]; then \
	$(MAKE) 'CYCFLAGS=$(CYCFLAGS) $(OPTFLAG)' NOOPT=yes $@; \
	else $(MAKE) NOOPT=yes $@; fi)
endif

# FIX: the wchar stuff should be moved to the cycspecs file
bin/genfiles/install_path.c: $(CYCDIR)/Makefile.inc
	 (echo "char *Carch = \"$(build)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Cversion = \"$(VERSION)\";"; \
	  echo "int Wchar_t_unsigned = $(WCHAR_T_UNSIGNED);"; \
	  echo "int Sizeof_wchar_t = $(SIZEOF_WCHAR_T);") > $@

bin/lib/$(CYCLIB): bin/lib/cyc-lib/$(build)/cyc_setjmp.h
bin/lib/$(CYCLIB): \
  build/$(build)/include/cstubs.$(O) \
  build/$(build)/include/cycstubs.$(O) \
  $(addprefix build/$(build)/, $(O_BOOT_LIBS)) \
  $(addprefix build/$(build)/, $(O_LIBS))
	-$(RM) $@
	ar rc $@ \
	  $(addprefix build/$(build)/, $(O_LIBS)) \
	  $(addprefix build/$(build)/, $(O_BOOT_LIBS)) \
	  build/$(build)/include/cstubs.$(O) \
	  build/$(build)/include/cycstubs.$(O)
	$(RANLIB) $@

ifneq ($(build),$(target))
bin/lib/cyc-lib/$(target)/$(CYCLIB): bin/lib/cyc-lib/$(target)/cyc_setjmp.h
bin/lib/cyc-lib/$(target)/$(CYCLIB): \
  build/$(target)/include/cstubs.$(O) \
  build/$(target)/include/cycstubs.$(O) \
  $(addprefix build/$(target)/, $(O_LIBS)) \
  $(addprefix build/$(target)/, $(O_BOOT_LIBS))
	-$(RM) $@
	ar rc $@ \
	  $(addprefix build/$(target)/, $(O_LIBS)) \
	  $(addprefix build/$(target)/, $(O_BOOT_LIBS)) \
	  build/$(target)/include/cstubs.$(O) \
	  build/$(target)/include/cycstubs.$(O)
	$(RANLIB) $@
endif

bin/lib/libcyc_a.a: \
  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
  $(addprefix lib/, $(A_LIBS)) \
  build/$(build)/include/cstubs_a.$(O) \
  build/$(build)/include/cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
	  $(addprefix lib/, $(A_LIBS)) \
	  build/$(build)/include/cstubs_a.$(O) \
	  build/$(build)/include/cycstubs_a.$(O)
	$(RANLIB) $@

bin/lib/libcyc_pg.a: \
  $(addprefix bin/genfiles/, $(PG_BOOT_LIBS)) \
  $(addprefix lib/, $(PG_LIBS)) \
  build/$(build)/include/cstubs_pg.$(O) \
  build/$(build)/include/cycstubs_pg.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

bin/lib/libcyc_nocheck.a: \
  $(addprefix bin/genfiles/, $(NOCHECK_BOOT_LIBS)) \
  $(addprefix lib/, $(NOCHECK_LIBS)) \
  build/$(build)/include/cstubs_nocheck.$(O) \
  build/$(build)/include/cycstubs_nocheck.$(O)
	-$(RM) $@
	ar rc $@ $^
	$(RANLIB) $@

bin/lib/cyc-lib/%/nogc.a: bin/genfiles/malloc.c
bin/lib/cyc-lib/%/nogc.a: build/%/nogc.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

# FIX: No idea why this rule below is needed given the rule above
ifneq ($(build),$(target))
bin/lib/cyc-lib/$(target)/nogc.a: bin/genfiles/malloc.c
bin/lib/cyc-lib/$(target)/nogc.a: build/$(target)/nogc.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@
endif

bin/lib/cyc-lib/$(build)/nogc_a.a: bin/genfiles/malloc.c
bin/lib/cyc-lib/$(build)/nogc_a.a: \
  build/$(build)/nogc_a.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

bin/lib/cyc-lib/$(build)/nogc_pg.a: bin/genfiles/malloc.c
bin/lib/cyc-lib/$(build)/nogc_pg.a: \
  build/$(build)/nogc_pg.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

bin/lib/cyc-lib/$(build)/runtime_cyc.a: \
  $(addprefix build/$(build)/, $(O_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix build/$(build)/, $(O_RUNTIME))
	$(RANLIB) $@

ifneq ($(build),$(target))
bin/lib/cyc-lib/$(target)/runtime_cyc.a: \
  $(addprefix build/$(target)/, $(O_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix build/$(target)/, $(O_RUNTIME))
	$(RANLIB) $@
endif

bin/lib/cyc-lib/$(build)/runtime_cyc_a.a: \
  $(addprefix build/$(build)/, $(A_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix build/$(build)/, $(A_RUNTIME))
	$(RANLIB) $@

bin/lib/cyc-lib/$(build)/runtime_cyc_pg.a: \
  $(addprefix build/$(build)/, $(PG_RUNTIME))
	-$(RM) $@
	ar rc $@ $(addprefix build/$(build)/, $(PG_RUNTIME))
	$(RANLIB) $@

# Directory structure of the installed library.  (During boot,
# this is built in bin/lib.)
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

directory-structure:
	-mkdir -p bin/lib/cyc-lib/$(build)/include
	-mkdir -p build/$(build)   # for build temp files
ifneq ($(build),$(target))
	-mkdir -p bin/lib/cyc-lib/$(target)/include
	-mkdir -p build/$(target)  # for build temp files
endif

$(CYC_INCLUDE_H): $(CYCDIR)/bin/cyc-lib/cyc_include.h
	cp $< $@

# Build the cycspecs file.
# FIX: we are not using the specs file in the same way that
# gcc spec files are used, probably there should be just one
# entry for cyclone instead of separate entries for
# cyclone_target_cflags, cyclone_cc, etc.
bin/lib/cyc-lib/$(build)/cycspecs:
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

ifneq ($(build),$(target))
bin/lib/cyc-lib/$(target)/cycspecs:
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
endif

# The buildlib process creates many files; exactly what files depends
# on the architecture.  So, we give just three targets: cstubs.c,
# cycstubs.cyc, and cyc_setjmp.h, which are always made by buildlib no
# matter what.  Other files are silently created by side effect.
bin/lib/cyc-lib/$(build)/cyc_setjmp.h: bin/cyc-lib/libc.cys
	bin/buildlib -Bbin/lib/cyc-lib -d build/$(build)/include -setjmp > $@

ifneq ($(build),$(target))
bin/lib/cyc-lib/$(target)/cyc_setjmp.h: bin/cyc-lib/libc.cys
	bin/buildlib $(BTARGET) -Bbin/lib/cyc-lib -d build/$(target)/include -setjmp > $@
endif

#%/cstubs.c: %/cycstubs.cyc
build/$(build)/include/cstubs.c: build/$(build)/include/cycstubs.cyc

ifneq ($(build),$(target))
build/$(target)/include/cstubs.c: build/$(target)/include/cycstubs.cyc
endif

# FIX: the rule creates a bunch of header files but the header
# files aren't targets, so, dependencies are wrong and make can
# get confused.
build/$(build)/include/cycstubs.cyc: bin/cyc-lib/libc.cys
	bin/buildlib -Bbin/lib/cyc-lib -d $(@D) $<
#	cp -r $(@D)/* bin/lib/cyc-lib/$(build)/include
	for i in `(cd build/$(build)/include; find * -type d)`;\
	  do mkdir -p bin/lib/cyc-lib/$(build)/include/$$i; done
	for i in `(cd build/$(build)/include; find * -name '*.h')`;\
	  do cp build/$(build)/include/$$i bin/lib/cyc-lib/$(build)/include/$$i; done

ifneq ($(build),$(target))
build/$(target)/include/cycstubs.cyc: bin/cyc-lib/libc.cys
	bin/buildlib $(BTARGET) -Bbin/lib/cyc-lib -d $(@D) $<
#	cp -r $(@D)/* bin/lib/cyc-lib/$(target)/include
	for i in `(cd build/$(target)/include; find * -type d)`;\
	  do mkdir -p bin/lib/cyc-lib/$(target)/include/$$i; done
	for i in `(cd build/$(target)/include; find * -name '*.h')`;\
	  do cp build/$(target)/include/$$i bin/lib/cyc-lib/$(target)/include/$$i; done
endif

build/$(build)/include/precore_c.h: \
  include/core.h \
  bin/cyclone$(EXE) \
  bin/lib/cyc-lib/$(build)/cycspecs
	bin/cyclone$(EXE) -Iinclude -Bbin/lib/cyc-lib -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -nocyc_setjmp -x cyc -o $@ $<

ifneq ($(build),$(target))
build/$(target)/include/precore_c.h: \
  include/core.h \
  bin/cyclone$(EXE) \
  bin/lib/cyc-lib/$(target)/cycspecs
	bin/cyclone$(EXE) $(BTARGET) -Iinclude -Bbin/lib/cyc-lib -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -nocyc_setjmp -x cyc -o $@ $<
endif

build/$(build)/include/cstubs.$(O): build/$(build)/include/cstubs.c build/$(build)/include/precore_c.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

ifneq ($(build),$(target))
build/$(target)/include/cstubs.$(O): build/$(target)/include/cstubs.c build/$(target)/include/precore_c.h
	$(CC) $(BTARGET) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
endif

build/$(build)/include/cycstubs.$(O): build/$(build)/include/cycstubs.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -save-c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -c -o $@ $<

ifneq ($(build),$(target))
build/$(target)/include/cycstubs.$(O): build/$(target)/include/cycstubs.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) $(BTARGET) -save-c -Iinclude -Ibin/lib/cyc-lib/$(build)/include -Bbin/lib/cyc-lib -c -o $@ $<
endif

bin/lib/cyc-lib/$(build)/gc.a: gc/.libs/libgc.a
	cp -p $< $@

gc/.libs/libgc.a:
	$(MAKE) -C gc libgc.la CC="$(CC)" CFLAGS="$(CFLAGS)"

# gc/gc.a:
#	$(MAKE) -C gc CC="$(CC)" gc.a CFLAGS="$(CFLAGS) -O -I./include -DATOMIC_UNCOLLECTABLE -DNO_SIGNALS -DNO_EXECUTE_PERMISSION -DALL_INTERIOR_POINTERS -DNO_DEBUGGING -DDONT_ADD_BYTE_AT_END"

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
	$(SHELL) config/cyc_install bin/lib/* $(LIB_INSTALL)
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
           (rm -rf $(UNUPDATEDIR) && mv $(UNUPDATEDIR)-backup $(UNUPDATEDIR) \
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
	@rm -rf $(UNUPDATEDIR)
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
	@rm -rf build/boot_test_src_compiler
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
         CYCFLAGS="-L$(CYCDIR)/bin/lib -g -save-c -pp -I$(CYCDIR)/include -B$(CYCDIR)/bin/lib/cyc-lib"
# The -I and -B flags are explained in Makefile_libsrc and should be
# kept in sync with the settings there
test_boot:
	$(MAKE) -C tests\
         CYCC=$(CYCDIR)/build/boot/cyclone$(EXE)\
         CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)\
         CYCFLAGS="$(addprefix -I$(CYCDIR)/build/boot/, . include) $(addprefix -I$(CYCDIR)/, lib src include) -B$(CYCDIR)/build/boot -B$(CYCDIR)/lib -g -save-c -pp"\
	 LDFLAGS="-L$(CYCDIR)/build/boot -B$(CYCDIR)/bin/lib/cyc-lib -v"

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

clean_genfiles:
	$(RM) bin/genfiles/*.$(O)
	$(RM) bin/genfiles/install_path.c

clean_nogc: clean_test clean_build clean_genfiles
	$(MAKE) -C tools/bison  clean
	$(MAKE) -C tools/cyclex clean
	$(MAKE) -C tools/flex   clean
	$(MAKE) -C tools/rewrite clean
	$(MAKE) -C tools/aprof  clean
	$(MAKE) -C tools/errorgen clean
	$(MAKE) -C tests        clean
#	$(MAKE) -C doc          clean
	$(MAKE) -C lib/xml      clean
	$(RM) -r bin/lib
	$(RM) $(addprefix bin/, $(addsuffix $(EXE), cyclone cyclone_a cycdoc buildlib cycbison cyclex cycflex aprof rewrite errorgen))
	$(RM) *~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
