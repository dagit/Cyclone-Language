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
CYC_LIB_PATH := $(CYCDIR)/bin/cyc-lib
CYC_INC_PATH := $(CYCDIR)/lib

build: $(CYC_LIB_PATH)/gc.a cyclone tools aprof libs 

cyclone: include/cstdio.h include/csignal.h
	$(MAKE) -C bin/genfiles install 
tools:
	$(MAKE) -C tools/bison  install 
	$(MAKE) -C tools/cyclex install 
	$(MAKE) -C tools/flex   install 
aprof:
	$(MAKE) -C bin/genfiles install_a 
	$(MAKE) -C tools/aprof  install 
libs:
ifndef NO_XML_LIB
	$(MAKE) -C lib/xml install 
endif
# Non-null/bounds checked version of the library -- not built by default
nocheck:
	$(MAKE) -C bin/genfiles install_nocheck 

.PHONY: tools cyclone aprof libs nocheck

$(CYC_LIB_PATH)/gc.a:
	$(MAKE) -C gc CC="$(CC)" gc.a CFLAGS="$(CFLAGS) -O -I./include -DATOMIC_UNCOLLECTABLE -DNO_SIGNALS -DNO_EXECUTE_PERMISSION -DALL_INTERIOR_POINTERS -DSILENT -DNO_DEBUGGING -DDONT_ADD_BYTE_AT_END"
	ln gc/gc.a $@

# Kludge to use architecture-specific constants -- we hope buildlib comes soon
# The resulting files must *not* be used when cross-compiling.
include/cstdio.h: include/cstdio.h_in include/arch/$(ARCH).h
	-$(RM) $@
	echo "#ifndef _STDIO_H" >> $@
	echo "#define _STDIO_H" >> $@
	echo "#define _EXTRACT_STDIOCONSTS" >> $@
	cat include/arch/$(ARCH).h >> $@
	echo "#undef _EXTRACT_STDIOCONSTS" >> $@
	cat $< >> $@
include/csignal.h: include/csignal.h_in include/arch/$(ARCH).h
	-$(RM) $@
	echo "#ifndef _SIGNAL_H" >> $@
	echo "#define _SIGNAL_H" >> $@
	echo "#define _EXTRACT_SIGNALCONSTS" >> $@
	cat include/arch/$(ARCH).h >> $@
	echo "#undef _EXTRACT_SIGNALCONSTS" >> $@
	cat $< >> $@

# Store the compiler, libraries, and tools in the user-defined directories.
# Also, keep a record of what was copied for later uninstall.
install: build inc_install lib_install bin_install
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
	$(SHELL) config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) bin/cycflex$(EXE) $(BIN_INSTALL)
bin_uninstall:
	$(SHELL) config/cyc_install -u $(BIN_INSTALL)
else
bin_install bin_uninstall:
	@(echo "no bin directory specified"; exit 1)
endif
ifdef LIB_INSTALL
lib_install:
	$(SHELL) config/cyc_install bin/cyc-lib/* $(LIB_INSTALL)
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
# We override BUILD_DIR and CYCFLAGS for many nefarious purposes:
#   cross-compiling, bootstrapping, profiling, ...
DO_LIBSRC=$(MAKE) -C $(BUILDDIR) -f $(CYCDIR)/Makefile_libsrc 

cyclone_src:
	-mkdir $(BUILDDIR) >& /dev/null
	$(DO_LIBSRC) all
lib_src:
	-mkdir $(BUILDDIR) >& /dev/null
	$(DO_LIBSRC) libs
cfiles:
	-mkdir $(BUILDDIR) >& /dev/null
	$(DO_LIBSRC) CYCFLAGS="$(CYCFLAGS) -stopafter-toc" cfiles

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
	@cmp -s $(ARCHDIR)/lib/cstubs.c lib/cstubs.c \
              || echo $(XS) lib/cstubs.c CHANGED
	@cmp -s $(ARCHDIR)/lib/nogc.c lib/nogc.c\
              || echo $(XS) lib/nogc.c CHANGED
	@test ! -e lib/include/cyc_include.h\
	      || cmp -s bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h\
              || echo $(XS) cyc-lib/include/cyc_include.h CHANGED

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
	@cmp -s lib/cstubs.c $(ARCHDIR)/lib/cstubs.c\
               || (echo UPDATING $(ARCHDIR)lib/cstubs.c;\
                   cp lib/cstubs.c $(ARCHDIR)/lib/cstubs.c)
	@cmp -s lib/nogc.c $(ARCHDIR)/lib/nogc.c\
               || (echo UPDATING $(ARCHDIR)/lib/nogc.c;\
                   cp lib/nogc.c $(ARCHDIR)/lib/nogc.c)
ifeq ($(UPDATEARCH),$(ARCH))
	@for i in $(CYCLONE_H);\
           do (test ! -e lib/$$i || cmp -s lib/$$i include/$$i\
               || (echo UPDATING lib/$$i;\
                   cp lib/$$i include/$$i)) done
	@(cd lib; for i in arch/*.h;\
	   do (cmp -s $$i ../include/$$i\
               || (echo UPDATING include/$$i;\
                    cp $$i ../include/$$i)) done)
	@test ! -e lib/include/cyc_include.h\
               || cmp -s lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h\
               || (echo UPDATING cyc-lib/include/cyc_include.h;\
                 cp lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h)
endif
	@if [ "$(UPDATEARCH)" = "$(PATCH_ARCH)" ]; then\
	  for arch in $(ALL_ARCHS); do\
	    if [ -f "bin/genfiles/$$arch.patch" ]; then\
	      echo "PATCHIFYING $$arch";\
	      $(MAKE) -s -C bin/genfiles $$arch.patch && \
	      $(RM) -rf bin/genfiles/$$arch;\
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
	 UPDATEARCH=$(PATCH_ARCH)\
	 CYCFLAGS="$(CYCFLAGS) -use-cpp '$(addprefix $(CYCDIR)/config/, cyccpp arch/$(ARCH) arch/$(PATCH_ARCH))'";\
	else\
	  $(MAKE) update;\
	fi;
	@for arch in $(ALL_ARCHS); do\
	  if [ "$$arch" != "$(PATCH_ARCH)" ]; then\
	    if [ "$$arch" != "$(ARCH)" ]; then\
	      $(MAKE) update\
	 NODEPS=X\
	 BUILDDIR=build/$$arch\
	 UPDATEARCH=$$arch\
	 CYCFLAGS="$(CYCFLAGS) -use-cpp '$(addprefix $(CYCDIR)/config/, cyccpp arch/$(ARCH) arch/$$arch)'";\
	    else\
	      $(MAKE) update;\
	    fi;\
	  fi;\
	done

# only update development architectures
update_devel_archs:
	$(MAKE) update_all_archs ALL_ARCHS="$(DEVEL_ARCHS)"

# a little testing (much more is in the tests directory)
test:
	$(MAKE) -C tests CYCFLAGS="-g -save-c -pp"
test_bin:
	$(MAKE) -C tests\
         CYCC=$(CYCDIR)/bin/cyclone$(EXE)\
         CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)\
         CYCFLAGS="-g -save-c -pp -I$(CYCDIR)/include -B$(CYCDIR)/bin/cyc-lib"
test_boot:
	$(MAKE) -C tests\
         CYCC=$(CYCDIR)/build/boot/cyclone$(EXE)\
         CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)\
         CYCFLAGS="-g -save-c -pp -I$(CYCDIR)/lib -B$(CYCDIR)/lib"

clean_test:
	$(MAKE) -C tests clean

# To do: a much safer and less kludgy way to clean build!
# To do: a way to clean individual directories in build.
clean_build:
	mv build/CVS .build_CVS
	$(RM) -rf build/*
	mv .build_CVS build/CVS

clean_nogc: clean_test clean_build
	$(MAKE) -C tools/bison  clean
	$(MAKE) -C tools/cyclex clean
	$(MAKE) -C tools/flex   clean
	$(MAKE) -C tools/aprof  clean
	$(MAKE) -C bin/genfiles clean
	$(MAKE) -C tests        clean
	$(MAKE) -C doc          clean
	$(MAKE) -C lib/xml      clean
	$(RM) bin/cyc-lib/*libcyc*.a
	$(RM) $(addprefix bin/, $(addsuffix $(EXE), cyclone cycdoc buildlib cycbison cyclex cycflex aprof))
	$(RM) *~ amon.out
	$(RM) include/cstdio.h include/csignal.h 

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
	$(RM) bin/cyc-lib/*gc*.a
