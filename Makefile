# The following instructions assume you have not added any files to
# src or lib.  If you have, go update Makefile.inc.in in the obvious manner!

# To update to a new version of the compiler, here's the boostrap sequence
#   1. make
#   2. make cyclone_src
#   3. make update
#   4. make
# It is strongly advised that you precede step 3 with the boostrap-checking
# facilities provided by overriding CYCC, BUILDDIR, INC_PATH, and LIB_PATH.

# To use a new version of the compiler without nuking the C files used to
# bootstrap:
#   1. make all
#   2. make cyclone_src
# There is now a build/boot/cyclone.exe that you can use/move as you wish.

include Makefile.inc

export CYCDIR CC CYCFLAGS UPDATEARCH

ifndef ARCH
$(error "Must have ARCH variable defined -- perhaps forgot to run ./configure")
endif

CYC_INCLUDE_H := bin/lib/cyc-lib/cyc_include.h

all: cyclone tools libs 

# Print version to standard output -- used by makedist
version:
	@echo $(VERSION)

cyclone: \
	$(CYC_INCLUDE_H) \
	bin/lib/cyc-lib/$(ARCH)/gc.a \
	bin/lib/$(CYCBOOTLIB) \
	$(addprefix bin/, cyclone$(EXE) cycdoc$(EXE) buildlib$(EXE)) \
	bin/lib/cyc-lib/$(ARCH)/cycspecs \
	$(addprefix bin/lib/cyc-lib/$(ARCH)/, nogc.a $(RUNTIME).$(O)) \
	bin/lib/cyc-lib/$(ARCH)/cyc_setjmp.h \
	bin/lib/$(CYCLIB)

tools:
	$(MAKE) -C tools/bison  install 
	$(MAKE) -C tools/cyclex install 
	$(MAKE) -C tools/flex   install 
	$(MAKE) -C tools/rewrite install

ifndef NO_XML_LIB
libs:
	$(MAKE) -C lib/xml install 
a_libs:
	$(MAKE) -C lib/xml a_install
else
libs:
a_libs:
endif

aprof: $(CYC_INCLUDE_H) \
  bin/lib/libcycboot_a.a \
  bin/lib/libcyc_a.a \
  $(addprefix bin/lib/cyc-lib/$(ARCH)/, nogc_a.a $(RUNTIME)_a.$(O)) \
  a_libs
	$(MAKE) -C tools/aprof install

cyclone_a: aprof bin/cyclone_a$(EXE)

cyclone_pg: bin/cyclone_pg$(EXE)

gprof: bin/lib/libcyc_pg.a \
  $(addprefix bin/lib/cyc-lib/$(ARCH)/, nogc_pg.a $(RUNTIME)_pg.$(O))

nocheck: bin/lib/libcyc_nocheck.a \
  $(addprefix bin/lib/cyc-lib/$(ARCH)/, nogc.a $(RUNTIME).$(O))

.PHONY: all tools cyclone aprof gprof libs nocheck

# Executables for the bin directory
bin/cyclone$(EXE): \
  $(addprefix bin/genfiles/, $(O_SRCS) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(ARCH)/$(RUNTIME).$(O) \
  bin/lib/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

bin/cyclone_a$(EXE): \
  $(addprefix bin/genfiles/, $(A_SRCS) install_path.$(O)) \
  bin/lib/libcycboot_a.a \
  bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_a.$(O) \
  bin/lib/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

bin/cyclone_pg$(EXE): \
  $(addprefix bin/genfiles/, $(PG_SRCS) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_pg.$(O) \
  bin/lib/cyc-lib/$(ARCH)/gc.a
	$(CC) -pg -o $@ $^ $(LDFLAGS)

bin/cycdoc$(EXE): \
  $(addprefix bin/genfiles/, $(addsuffix .$(O), $(CYCDOC_SRCS)) install_path.$(O)) \
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(ARCH)/$(RUNTIME).$(O) \
  bin/lib/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

bin/buildlib$(EXE): \
  $(addprefix bin/genfiles/, $(addsuffix .$(O), $(BUILDLIB_SRCS)) install_path.$(O))\
  bin/lib/$(CYCBOOTLIB) \
  bin/lib/cyc-lib/$(ARCH)/$(RUNTIME).$(O) \
  bin/lib/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)


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
#         precore_c.h         # precore_c, cstubs, and cycstubs are just
#         cstubs.{c,o}        #  an artifact of the build process, should
#         cycstubs.{cyc,o}    #  properly be deleted before install
#       nogc.a
#       runtime_cyc.o
#
# Note, every file in this hierarchy has $(CYC_INCLUDE_H) as a
# prerequisite (except for cyc_include.h).  This is because the rule
# for cyc_include.h causes the complete directory structure to be
# mkdir'd.  The other files actually just need their parent directory
# to exist, they don't depend cyc_include.h.  However, if they instead
# used their parent directory as a prerequisite, this would lead to
# unnecessary rebuilds, because the mod time of a directory changes
# when, e.g., you add a file to it.

bin/lib/$(CYCBOOTLIB): $(CYC_INCLUDE_H)
bin/lib/$(CYCBOOTLIB): \
  $(addprefix bin/genfiles/, $(O_BOOT_LIBS)) \
  bin/genfiles/boot_cstubs.$(O) \
  bin/genfiles/boot_cycstubs.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(O_BOOT_LIBS)) \
	  bin/genfiles/boot_cstubs.$(O) \
	  bin/genfiles/boot_cycstubs.$(O)
	$(RANLIB) $@

bin/lib/libcycboot_a.a: \
  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
  bin/genfiles/boot_cstubs_a.$(O) \
  bin/genfiles/boot_cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
	  bin/genfiles/boot_cstubs_a.$(O) \
	  bin/genfiles/boot_cycstubs_a.$(O)
	$(RANLIB) $@

%.$(O): %.cyc bin/cyclone$(EXE)
	bin/cyclone$(EXE) -c -Iinclude -Ibin/lib/cyc-lib/$(ARCH)/include -Bbin/lib/cyc-lib -o $@ $<

%_a.$(O): %.c
	$(CC) -c -o $@ -DCYC_REGION_PROFILE $(CFLAGS) $<

%_a.$(O): %.cyc cyclone
	bin/cyclone$(EXE) -pa -c -Iinclude -Ibin/lib/cyc-lib/$(ARCH)/include -Bbin/lib/cyc-lib -o $@ $<

%_pg.$(O): %.c
	$(CC) -c -o $@ -pg $(CFLAGS) $<

%_pg.$(O): %.cyc cyclone
	bin/cyclone$(EXE) -pg -c -Iinclude -Ibin/lib/cyc-lib/$(ARCH)/include -Bbin/lib/cyc-lib -o $@ $<

%_nocheck.$(O): %.c
	$(CC) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $(CFLAGS) $<

%_nocheck.$(O): %.cyc cyclone
	bin/cyclone$(EXE) --nochecks -c -Iinclude -Ibin/lib/cyc-lib/$(ARCH)/include -Bbin/lib/cyc-lib -o $@ $<

bin/genfiles/install_path.c: $(CYCDIR)/Makefile.inc
	 (echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Ccomp = \"$(CC)\";"; \
	  echo "char *Cversion = \"$(VERSION)\";"; \
	  echo "int Wchar_t_unsigned = $(WCHAR_T_UNSIGNED);"; \
	  echo "int Sizeof_wchar_t = $(SIZEOF_WCHAR_T);") > $@

bin/lib/$(CYCLIB): $(CYC_INCLUDE_H)
bin/lib/$(CYCLIB): bin/lib/cyc-lib/$(ARCH)/cyc_setjmp.h
bin/lib/$(CYCLIB): \
  $(addprefix lib/, $(O_LIBS)) \
  $(addprefix bin/genfiles/, $(O_BOOT_LIBS)) \
  bin/lib/cyc-lib/$(ARCH)/include/cstubs.$(O) \
  bin/lib/cyc-lib/$(ARCH)/include/cycstubs.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix lib/, $(O_LIBS)) \
	  $(addprefix bin/genfiles/, $(O_BOOT_LIBS)) \
	  bin/lib/cyc-lib/$(ARCH)/include/cstubs.$(O) \
	  bin/lib/cyc-lib/$(ARCH)/include/cycstubs.$(O)
	$(RANLIB) $@

bin/lib/libcyc_a.a: \
  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
  $(addprefix lib/, $(A_LIBS)) \
  bin/lib/cyc-lib/$(ARCH)/include/cstubs_a.$(O) \
  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_a.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(A_BOOT_LIBS)) \
	  $(addprefix lib/, $(A_LIBS)) \
	  bin/lib/cyc-lib/$(ARCH)/include/cstubs_a.$(O) \
	  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_a.$(O)
	$(RANLIB) $@

bin/lib/libcyc_pg.a: \
  $(addprefix bin/genfiles/, $(PG_BOOT_LIBS)) \
  $(addprefix lib/, $(PG_LIBS)) \
  bin/lib/cyc-lib/$(ARCH)/include/cstubs_pg.$(O) \
  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_pg.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(PG_BOOT_LIBS)) \
	  $(addprefix lib/, $(PG_LIBS)) \
	  bin/lib/cyc-lib/$(ARCH)/include/cstubs_pg.$(O) \
	  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_pg.$(O)
	$(RANLIB) $@

bin/lib/libcyc_nocheck.a: \
  $(addprefix bin/genfiles/, $(NOCHECK_BOOT_LIBS)) \
  $(addprefix lib/, $(NOCHECK_LIBS)) \
  bin/lib/cyc-lib/$(ARCH)/include/cstubs_nocheck.$(O) \
  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_nocheck.$(O)
	-$(RM) $@
	ar rc $@ \
	  $(addprefix bin/genfiles/, $(NOCHECK_BOOT_LIBS)) \
	  $(addprefix lib/, $(NOCHECK_LIBS)) \
	  bin/lib/cyc-lib/$(ARCH)/include/cstubs_nocheck.$(O) \
	  bin/lib/cyc-lib/$(ARCH)/include/cycstubs_nocheck.$(O)
	$(RANLIB) $@

bin/lib/cyc-lib/$(ARCH)/nogc.a: $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/nogc.a: \
  bin/genfiles/nogc.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

bin/lib/cyc-lib/$(ARCH)/nogc_a.a: $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/nogc_a.a: \
  bin/genfiles/nogc_a.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

bin/lib/cyc-lib/$(ARCH)/nogc_pg.a: $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/nogc_pg.a: \
  bin/genfiles/nogc_pg.$(O)
	-$(RM) $@
	ar rc $@ $<
	$(RANLIB) $@

bin/lib/cyc-lib/$(ARCH)/$(RUNTIME).$(O): $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/$(RUNTIME).$(O): \
  bin/genfiles/$(RUNTIME).$(O)
	cp $< $@

bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_a.$(O): $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_a.$(O): \
  bin/genfiles/$(RUNTIME)_a.$(O)
	cp $< $@

bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_pg.$(O): $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/$(RUNTIME)_pg.$(O): \
  bin/genfiles/$(RUNTIME)_pg.$(O)
	cp $< $@

# The rule for creating cyc_include.h creates as a side effect
# the entire cyclone lib directory structure; see above.
bin/lib/cyc-lib/cyc_include.h: $(CYCDIR)/bin/cyc-lib/cyc_include.h
	-mkdir bin/lib
	-mkdir bin/lib/cyc-lib
	-mkdir bin/lib/cyc-lib/$(ARCH)
	-mkdir bin/lib/cyc-lib/$(ARCH)/include
	cp $< $@

bin/lib/cyc-lib/$(ARCH)/cycspecs: $(CYC_INCLUDE_H)
	${CYCDIR}/config/buildspecs > $@

# The buildlib process creates many files; exactly what files depends
# on the architecture.  So, we give just three targets: cstubs.c,
# cycstubs.cyc, and cyc_setjmp.h, which are always made by buildlib no
# matter what.
$(addprefix bin/lib/cyc-lib/$(ARCH)/include/, cstubs.c cycstubs.cyc) \
bin/lib/cyc-lib/$(ARCH)/cyc_setjmp.h: \
  $(CYC_INCLUDE_H) \
  bin/cyc-lib/libc.cys
	bin/buildlib -d bin/lib/cyc-lib/$(ARCH)/include -setjmp > bin/lib/cyc-lib/$(ARCH)/cyc_setjmp.h
	bin/buildlib -d bin/lib/cyc-lib/$(ARCH)/include bin/cyc-lib/libc.cys

bin/lib/cyc-lib/$(ARCH)/include/precore_c.h: $(CYC_INCLUDE_H)
bin/lib/cyc-lib/$(ARCH)/include/precore_c.h: \
  include/core.h \
  bin/cyclone$(EXE) \
  bin/lib/cyc-lib/$(ARCH)/cycspecs
	bin/cyclone$(EXE) -Iinclude -Bbin/lib/cyc-lib -stopafter-toc -pp -D_CYC_GENERATE_PRECORE_C_ -nocyc -noremoveunused -noexpandtypedefs -nocyc_setjmp -o $@ -x cyc $<

bin/lib/cyc-lib/$(ARCH)/include/cstubs.$(O): \
  bin/lib/cyc-lib/$(ARCH)/include/cstubs.c \
  bin/lib/cyc-lib/$(ARCH)/include/precore_c.h
	$(CC) $(CFLAGS) -c -o $@ $<

bin/lib/cyc-lib/$(ARCH)/include/cycstubs.$(O): \
  bin/lib/cyc-lib/$(ARCH)/include/cycstubs.cyc \
  bin/cyclone$(EXE)
	bin/cyclone$(EXE) -save-c -Iinclude -Bbin/lib/cyc-lib -c -o $@ $<

bin/lib/cyc-lib/$(ARCH)/gc.a: gc/.libs/libgc.a $(CYC_INCLUDE_H)
	cp -p $< $@

gc/.libs/libgc.a:
	$(MAKE) -C gc libgc.la CC="$(CC)"

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
	$(SHELL) config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) bin/cycflex$(EXE) bin/rewrite$(EXE) bin/aprof$(EXE) $(BIN_INSTALL)
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
.PHONY: inc_uninstall lib_uninstall bin_uninstall

UPDATEARCH=$(ARCH)
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

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
XS=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
cmp: 
	@for i in $(UPDATE_SRCS);\
	  do (cmp -s $(GENDIR)/$$i $(BUILDDIR)/$$i\
	      || echo $(XS) $$i CHANGED) done
	@for i in $(C_BOOT_LIBS);\
	  do (cmp -s $(GENDIR)/$$i $(BUILDDIR)/$$i\
	      || echo $(XS) $$i CHANGED) done
	@cmp -s $(GENDIR)/precore_c.h $(BUILDDIR)/precore_c.h\
	      || echo $(XS) precore_c.h CHANGED
	@for i in $(CYCLONE_H);\
	  do (test ! -e lib/$$i || cmp -s include/$$i lib/$$i\
              || echo $(XS) lib/$$i CHANGED) done
	@cmp -s $(GENDIR)/$(C_RUNTIME) lib/$(C_RUNTIME)\
              || echo $(XS) $(C_RUNTIME) CHANGED
	@cmp -s $(GENDIR)/boot_cstubs.c lib/boot_cstubs.c\
              || echo $(XS) boot_cstubs.c CHANGED
	@cmp -s $(GENDIR)/boot_cycstubs.c $(BUILDDIR)/boot_cycstubs.c \
              || echo $(XS) boot_cycstubs.c CHANGED
	@cmp -s $(GENDIR)/nogc.c lib/nogc.c\
              || echo $(XS) nogc.c CHANGED
	@test ! -e lib/cyc_include.h\
	      || cmp -s bin/cyc-lib/cyc_include.h lib/cyc_include.h\
              || echo $(XS) cyc_include.h CHANGED
	@test ! -e lib/libc.cys\
	      || cmp -s bin/cyc-lib/libc.cys lib/libc.cys\
              || echo $(XS) libc.cys CHANGED


# This target updates what is in bin/genfiles and include.
# It would be "dangerous" to invoke this target if we did not have 
# version control.  Only updates changed files (makes cvs faster).
update: cfiles
	@for i in $(UPDATE_SRCS);\
	   do (cmp -s $(BUILDDIR)/$$i $(GENDIR)/$$i\
               || (echo UPDATING $(GENDIR)/$$i;\
	           cp $(BUILDDIR)/$$i $(GENDIR)/$$i)) done
	@for i in $(C_BOOT_LIBS);\
           do (cmp -s $(BUILDDIR)/$$i $(GENDIR)/$$i\
               || (echo UPDATING $(GENDIR)/$$i;\
	           cp $(BUILDDIR)/$$i $(GENDIR)/$$i)) done
	@cmp -s $(BUILDDIR)/precore_c.h $(GENDIR)/precore_c.h\
	       || (echo UPDATING $(GENDIR)/precore_c.h;\
	            cp $(BUILDDIR)/precore_c.h $(GENDIR)/precore_c.h)
	@cmp -s lib/$(C_RUNTIME) $(GENDIR)/$(C_RUNTIME)\
               || (echo UPDATING $(GENDIR)/$(C_RUNTIME);\
                   cp lib/$(C_RUNTIME) $(GENDIR)/$(C_RUNTIME))
	@cmp -s lib/boot_cstubs.c $(GENDIR)/boot_cstubs.c\
               || (echo UPDATING $(GENDIR)/boot_cstubs.c;\
                   cp lib/boot_cstubs.c $(GENDIR)/boot_cstubs.c)
	@cmp -s $(BUILDDIR)/boot_cycstubs.c $(GENDIR)/boot_cycstubs.c\
               || (echo UPDATING $(GENDIR)/boot_cycstubs.c;\
                   cp $(BUILDDIR)/boot_cycstubs.c $(GENDIR)/boot_cycstubs.c)
	@cmp -s lib/nogc.c $(GENDIR)/nogc.c\
               || (echo UPDATING $(GENDIR)/nogc.c;\
                   cp lib/nogc.c $(GENDIR)/nogc.c)
	@for i in $(CYCLONE_H);\
           do (test ! -e lib/$$i || cmp -s lib/$$i include/$$i\
               || (echo UPDATING include/$$i;\
                   mv lib/$$i include/$$i)) done
	@test ! -e lib/cyc_include.h\
               || cmp -s lib/cyc_include.h bin/cyc-lib/cyc_include.h\
               || (echo UPDATING bin/cyc-lib/cyc_include.h;\
                 mv lib/cyc_include.h bin/cyc-lib/cyc_include.h)
	@test ! -e lib/libc.cys\
               || cmp -s lib/libc.cys bin/cyc-lib/libc.cys\
               || (echo UPDATING bin/cyc-lib/libc.cys;\
                 mv lib/libc.cys bin/cyc-lib/libc.cys)

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
	$(MAKE) -C tests        clean
#	$(MAKE) -C doc          clean
	$(MAKE) -C lib/xml      clean
	$(RM) -r bin/lib
	$(RM) $(addprefix bin/, $(addsuffix $(EXE), cyclone cyclone_a cycdoc buildlib cycbison cyclex cycflex aprof rewrite))
	$(RM) *~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
