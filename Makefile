# The following instructions assume you have not added any files to
# src or lib.  If you have, go update Makefile.inc.in in the obvious manner!

# To update to a new version of the compiler, here's the boostrap sequence
#   1. make all
#   2. make cyclone_src
#   3. make update
#   4. make cyclone
# It is strongly advised that you precede step 3 with the boostrap-checking
# facilities provided by overriding CYCC, BUILDDIR, INC_PATH, and LIB_PATH.
# To update for other architectures, do make update_all_archs after you have
# reached a fixpoint.  Alternatively, do make update_devel_archs to update 
# linux and cygwin and wait until just before a release to do update_all_archs.

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

CYC_BIN_PATH := $(CYCDIR)/bin
CYC_LIB_PATH := $(CYCDIR)/bin/lib
CYC_INC_PATH := $(CYCDIR)/lib

all: $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/gc.a cyclone \
	$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/include \
	tools libs 

# FIX: a bug in buildlib forces us to use a relative path for -d
cyclone: $(CYC_LIB_PATH)
	$(MAKE) -C bin/genfiles install 

tools:
	$(MAKE) -C tools/bison  install 
	$(MAKE) -C tools/cyclex install 
	$(MAKE) -C tools/flex   install 
	$(MAKE) -C tools/rewrite install
aprof:
	$(MAKE) -C bin/genfiles install_a 
	$(MAKE) -C tools/aprof  install 
gprof:
	$(MAKE) -C bin/genfiles install_pg 
libs:
ifndef NO_XML_LIB
	$(MAKE) -C lib/xml install 
endif
# Non-null/bounds checked version of the library -- not built by default
nocheck:
	$(MAKE) -C bin/genfiles install_nocheck 

.PHONY: tools cyclone aprof libs nocheck

$(CYC_LIB_PATH):
	mkdir $@
	mkdir $@/cyc-lib
	mkdir $@/cyc-lib/$(ARCH)
	cp $(CYCDIR)/bin/cyc-lib/cyc_include.h $@/cyc-lib

$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/include: $(CYC_LIB_PATH) \
  $(CYCDIR)/bin/genfiles/$(ARCH).headers.tgz bin/cyc-lib/libc.cys
	-mkdir $@
	tar -z -xf bin/genfiles/$(ARCH).headers.tgz -C $@
	bin/buildlib -d $@ -finish bin/cyc-lib/libc.cys
	find $@ -name '*.i[BC]' -exec rm \{\} \;

$(CYCDIR)/bin/genfiles/$(ARCH).headers.tgz:
	$(CYCDIR)/bin/buildlib $(CYCDIR)/bin/cyc-lib/libc.cys -gather
	tar -z -cf $@ -C BUILDLIB.OUT .
	$(RM) -r BUILDLIB.OUT

$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/gc.a: gc/gc.a $(CYC_LIB_PATH)
	cp -p $< $@

gc/gc.a:
	$(MAKE) -C gc CC="$(CC)" gc.a CFLAGS="$(CFLAGS) -O -I./include -DATOMIC_UNCOLLECTABLE -DNO_SIGNALS -DNO_EXECUTE_PERMISSION -DALL_INTERIOR_POINTERS -DSILENT -DNO_DEBUGGING -DDONT_ADD_BYTE_AT_END"

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
	$(SHELL) config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) bin/cycflex$(EXE) bin/rewrite$(EXE) $(BIN_INSTALL)
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
ARCHDIR=bin/genfiles/$(UPDATEARCH)

# These build off the Cyclone source files, but do not replace anything in bin
# We override BUILDDIR and CYCFLAGS for many nefarious purposes:
#   cross-compiling, bootstrapping, profiling, ...
# We use the -r flag to eliminate built-in make rules.  This is needed
#   because of a circular dependence that arises when bootstrapping as with
#   make cyclone_src BUILDDIR=build/boot1 CYCC=`pwd`/build/boot/cyclone
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
	  do (cmp -s $(ARCHDIR)/src/$$i $(BUILDDIR)/$$i\
	      || echo $(XS) src/$$i CHANGED) done
	@for i in $(C_LIBS);\
	  do (cmp -s $(ARCHDIR)/lib/$$i $(BUILDDIR)/$$i\
	      || echo $(XS) lib/$$i CHANGED) done
	@cmp -s $(ARCHDIR)/lib/precore_c.h $(BUILDDIR)/precore_c.h\
	      || echo $(XS) lib/precore_c.h CHANGED
	@for i in $(CYCLONE_H);\
	  do (test ! -e lib/$$i || cmp -s include/$$i lib/$$i\
              || echo $(XS) lib/$$i CHANGED) done
	@cmp -s $(ARCHDIR)/lib/$(C_RUNTIME) lib/$(C_RUNTIME)\
              || echo $(XS) lib/$(C_RUNTIME) CHANGED
	@cmp -s $(ARCHDIR)/lib/cstubs.c $(BUILDDIR)/cstubs.c \
              || echo $(XS) lib/cstubs.c CHANGED
	@cmp -s $(ARCHDIR)/lib/nogc.c lib/nogc.c\
              || echo $(XS) lib/nogc.c CHANGED
	@test ! -e lib/cyc_include.h\
	      || cmp -s bin/cyc-lib/cyc_include.h lib/cyc_include.h\
              || echo $(XS) bin/cyc-lib/cyc_include.h CHANGED
	@test ! -e lib/libc.cys\
	      || cmp -s bin/cyc-lib/libc.cys lib/libc.cys\
              || echo $(XS) bin/cyc-lib/libc.cys CHANGED

# This target updates what is in bin/genfiles and include.
# It would be "dangerous" to invoke this target if we did not have 
# version control.  Only updates changed files (makes cvs faster).
update: cfiles
	 @if [ "$(UPDATEARCH)" = "$(PATCH_ARCH)" ]; then\
	   cd bin/genfiles; echo "UPDATING REFERENCE ARCH $(UPDATEARCH)";\
	   for arch in $(ALL_ARCHS); do\
	     ./extract_patch $(PATCH_ARCH) $$arch;\
	   done; cd ../..;\
	 else\
	   cd bin/genfiles; ./extract_patch $(PATCH_ARCH) $(UPDATEARCH);\
	   cd ../..;\
	 fi
	@for i in $(UPDATE_SRCS);\
	   do (cmp -s $(BUILDDIR)/$$i $(ARCHDIR)/src/$$i\
               || (echo UPDATING $(ARCHDIR)/src/$$i;\
	           cp $(BUILDDIR)/$$i $(ARCHDIR)/src/$$i)) done
	@for i in $(C_LIBS);\
           do (cmp -s $(BUILDDIR)/$$i $(ARCHDIR)/lib/$$i\
               || (echo UPDATING $(ARCHDIR)/lib/$$i;\
	           cp $(BUILDDIR)/$$i $(ARCHDIR)/lib/$$i)) done
	@cmp -s $(BUILDDIR)/precore_c.h $(ARCHDIR)/lib/precore_c.h\
	       || (echo UPDATING $(ARCHDIR)/lib/precore_c.h;\
	            cp $(BUILDDIR)/precore_c.h $(ARCHDIR)/lib/precore_c.h)
	@cmp -s lib/$(C_RUNTIME) $(ARCHDIR)/lib/$(C_RUNTIME)\
               || (echo UPDATING $(ARCHDIR)lib/$(C_RUNTIME);\
                   cp lib/$(C_RUNTIME) $(ARCHDIR)/lib/$(C_RUNTIME))
	@cmp -s $(BUILDDIR)/cstubs.c $(ARCHDIR)/lib/cstubs.c\
               || (echo UPDATING $(ARCHDIR)lib/cstubs.c;\
                   cp $(BUILDDIR)/cstubs.c $(ARCHDIR)/lib/cstubs.c)
	@cmp -s lib/nogc.c $(ARCHDIR)/lib/nogc.c\
               || (echo UPDATING $(ARCHDIR)/lib/nogc.c;\
                   cp lib/nogc.c $(ARCHDIR)/lib/nogc.c)
ifeq ($(UPDATEARCH),$(ARCH))
	@for i in $(CYCLONE_H);\
           do (test ! -e lib/$$i || cmp -s lib/$$i include/$$i\
               || (echo UPDATING lib/$$i;\
                   mv lib/$$i include/$$i)) done
	@test ! -e lib/cyc_include.h\
               || cmp -s lib/cyc_include.h bin/cyc-lib/cyc_include.h\
               || (echo UPDATING cyc-lib/cyc_include.h;\
                 mv lib/cyc_include.h bin/cyc-lib/cyc_include.h)
	@test ! -e lib/libc.cys\
               || cmp -s lib/libc.cys bin/cyc-lib/libc.cys\
               || (echo UPDATING cyc-lib/libc.cys;\
                 mv lib/libc.cys bin/cyc-lib/libc.cys)
endif
	 @if [ "$(UPDATEARCH)" = "$(PATCH_ARCH)" ]; then\
	   for arch in $(ALL_ARCHS); do\
	     if [ -f "bin/genfiles/$$arch.patch" ]; then\
	       echo "PATCHIFYING $$arch";\
	       $(MAKE) -s -C bin/genfiles $$arch.patch && \
	       if [ "$(ARCH)" != "$$arch" ]; then\
	         $(RM) -rf bin/genfiles/$$arch;\
	       fi;\
	     fi;\
	   done;\
	 elif [ -f "$(ARCHDIR).patch" ]; then\
	   echo "PATCHIFYING $(UPDATEARCH)";\
	   $(MAKE) -s -C bin/genfiles $(UPDATEARCH).patch && \
	   if [ "$(UPDATEARCH)" != "$(ARCH)" ]; then\
	     $(RM) -rf $(ARCHDIR);\
	   fi;\
	 fi

# This will compile (C files) and update for all supported architectures
#   and then compile and install for all supported architectures
#   We update the PATCH_ARCH first, so that the other updates
#   will create patches relative to the updated reference architecture.
# FIX: this will cause patched directories to be extracted twice; that
#   is, they will be extracted (and re-patchified) when we update the
#   reference architecture and then extracted and patchified again
#   when we update the patched directory itself.  Need to properly
#   communicate between update_all_archs and update for this to
#   work properly.
update_all_archs: 
	$(MAKE) -C bin/genfiles clean
	@if [ "$(PATCH_ARCH)" != "$(ARCH)" ]; then\
	  $(MAKE) update\
	 NODEPS=X\
	 BUILDDIR=build/$(PATCH_ARCH)\
	 UPDATEARCH=$(PATCH_ARCH);\
	else\
	  $(MAKE) update;\
	fi;
	@for arch in $(ALL_ARCHS); do\
	  if [ "$$arch" != "$(PATCH_ARCH)" ]; then\
	    if [ "$$arch" != "$(ARCH)" ]; then\
	      $(MAKE) update\
	 NODEPS=X\
	 BUILDDIR=build/$$arch\
	 UPDATEARCH=$$arch;\
	    else\
	      $(MAKE) update;\
	    fi;\
	  fi;\
	done

# only update development architectures
update_devel_archs:
	$(MAKE) update_all_archs ALL_ARCHS="$(DEVEL_ARCHS)"

# to add a new architecture
new_arch:
	cd bin/genfiles; echo "ADDING ARCH $(UPDATEARCH)";\
	./extract_patch $(PATCH_ARCH) $(UPDATEARCH);\
	if [ $$? = 1 ]; then \
	  mkdir $(UPDATEARCH); \
	  mkdir $(UPDATEARCH)/src; \
	  mkdir $(UPDATEARCH)/lib; \
	else \
	  echo "Error: architecture directory exists!"; \
	  exit 1;\
	fi;\
	touch $(UPDATEARCH).patch;\
	cd ../..; \
	$(MAKE) UPDATEARCH=$(UPDATEARCH) update;

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

clean_nogc: clean_test clean_build
	$(MAKE) -C tools/bison  clean
	$(MAKE) -C tools/cyclex clean
	$(MAKE) -C tools/flex   clean
	$(MAKE) -C tools/rewrite clean
	$(MAKE) -C tools/aprof  clean
	$(MAKE) -C bin/genfiles clean
	$(MAKE) -C tests        clean
#	$(MAKE) -C doc          clean
	$(MAKE) -C lib/xml      clean
	$(RM) -r bin/lib
	$(RM) $(addprefix bin/, $(addsuffix $(EXE), cyclone cycdoc buildlib cycbison cyclex cycflex aprof rewrite))
	$(RM) *~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
