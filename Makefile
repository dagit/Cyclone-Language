
# The following instructions assume you have not added any files to
# src or lib.  If you have, you must update Makefile.inc.in in the
# obvious manner.

# To update to a new version of the compiler, here's the boostrap sequence
#   1. make all
#   2. make cyclone_src
#   3. make update
#   4. make cyclone
# It is strongly advised that you precede step 3 with the boostrap-checking
# facilities provided by CYCC and OUT_PREFIX.
# To update for other architectures, do make update_all_archs after you have
# reached a fixpoint.  Alternatively, do make update_devel_archs to just
# update linux and cygwin; you can wait until just before a release to
# do update_all_archs.

# To use a new version of the compiler without nuking the C files used to
# bootstrap:
#   1. make all
#   2. make cyclone_src
# There is now a src/cyclone.exe that you can use/move as you wish.

include Makefile.inc

ifndef ARCH
$(error "Must have ARCH variable defined to properly compile; run configure")
endif

CYC_BIN_PATH := $(CYCDIR)/bin
CYC_LIB_PATH := $(CYCDIR)/bin/cyc-lib/
CYC_INC_PATH := $(CYCDIR)/lib

# These are passed to src and lib to control bootstrapping.  Override them
# on the command-line (using a path that will be interpreted in src and lib)
CYCC:=$(CYC_BIN_PATH)/$(CYCCOMP) 
OUT_PREFIX=

build: $(CYC_LIB_PATH)/gc.a cyclone tools aprof libs

# This target builds off the C files in bin/genfiles
cyclone:
	$(MAKE) install -C bin/genfiles

tools:
	$(MAKE) install -C tools/bison
	$(MAKE) install -C tools/cyclex
.PHONY: tools

libs:
ifndef NO_XML_LIB
	$(MAKE) install -C lib/xml
endif
.PHONY: libs

$(CYC_LIB_PATH)/gc.a:
	$(MAKE) -C gc gc.a CC=$(CC) CFLAGS=" $(CFLAGS) -O -I./include -DATOMIC_UNCOLLECTABLE -DNO_SIGNALS -DNO_EXECUTE_PERMISSION -DALL_INTERIOR_POINTERS -DSILENT -DNO_DEBUGGING -DDONT_ADD_BYTE_AT_END"
	ln gc/gc.a $@

# After building all of the source, install it in the user-defined 
# directories.  Also, keep a record of what was copied for later uninstall.
install: build inc_install lib_install bin_install
uninstall: inc_uninstall lib_uninstall bin_uninstall

ifdef INC_INSTALL
inc_install:
	$(SHELL) config/cyc_install include/* $(INC_INSTALL)
inc_uninstall:
	$(SHELL) config/cyc_install -u $(INC_INSTALL)
else
inc_install:
	@(echo "no include directory specified"; exit 1)
inc_uninstall:
	@(echo "no include directory specified"; exit 1)
endif

ifdef BIN_INSTALL
bin_install:
	$(SHELL) config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) $(BIN_INSTALL)
bin_uninstall:
	$(SHELL) config/cyc_install -u $(BIN_INSTALL)
else
bin_install:
	@(echo "no bin directory specified"; exit 1)
bin_uninstall:
	@(echo "no bin directory specified"; exit 1)
endif

ifdef LIB_INSTALL
lib_install:
	$(SHELL) config/cyc_install bin/cyc-lib/* $(LIB_INSTALL)
lib_uninstall:
	$(SHELL) config/cyc_install -u $(LIB_INSTALL)
else
lib_install:
	@(echo "no lib directory specified"; exit 1)
lib_uninstall:
	@(echo "no lib directory specified"; exit 1)
endif

# These build off the Cyclone source files, but do not replace anything in bin
# They are affected by CYCC and OUT_PREFIX.
cyclone_src: lib_src
	$(MAKE) -C src
lib_src:
	$(MAKE) -C lib

# Allocation profiler and its special version of the Cyclone library
aprof:
	$(MAKE) install_a -C bin/genfiles
	$(MAKE) install -C tools/aprof

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
diff: cyclone_src
	for i in $(UPDATE__SRCS); do (diff bin/genfiles/$(ARCH)/src/$$i src/$$i) done
	for i in $(C_LIBS); do (diff bin/genfiles/$(ARCH)/lib/$$i lib/$$i) done
	for i in $(CYCLONE_H); do (diff include/$$i lib/$$i) done
	diff bin/genfiles/$(ARCH)/lib/$(C_RUNTIME) lib/$(C_RUNTIME)
	diff bin/genfiles/$(ARCH)/lib/precore_c.h lib/precore_c.h
	diff bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h
	diff bin/genfiles/$(ARCH)/lib/nogc.c lib/nogc.c
	diff bin/genfiles/$(ARCH)/src/cycdoc.c src/cycdoc.c
	diff bin/genfiles/$(ARCH)/src/buildlib.c src/buildlib.c

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
cmp: 
	@for i in $(UPDATE_SRCS); do (cmp -s bin/genfiles/$(ARCH)/src/$$i src/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX src/$$i CHANGED) done
	@for i in $(C_LIBS); do (cmp -s bin/genfiles/$(ARCH)/lib/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@for i in $(CYCLONE_H); do (cmp -s include/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@cmp -s bin/genfiles/$(ARCH)/lib/$(C_RUNTIME) lib/$(C_RUNTIME) || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$(C_RUNTIME) CHANGED
	@cmp -s bin/genfiles/$(ARCH)/lib/precore_c.h lib/precore_c.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/precore_c.h CHANGED
	@cmp -s bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX cyc-lib/include/cyc_include.h CHANGED
	@cmp -s bin/genfiles/$(ARCH)/lib/nogc.c lib/nogc.c || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/nogc.c CHANGED
	@cmp -s bin/genfiles/$(ARCH)/src/cycdoc.c src/cycdoc.c || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX src/cycdoc.c CHANGED
	@cmp -s bin/genfiles/$(ARCH)/src/buildlib.c src/buildlib.c || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX src/buildlib.c CHANGED

# This target updates what is in bin/genfiles and include.
# It would be "dangerous" to invoke this target if we did not have 
# version control.  Only updates changed files (makes cvs faster).
ifndef TARGET
SRCDIR:=src
LIBDIR:=lib
ARCHDIR:=bin/genfiles/$(ARCH)
UPDATEARCH:=$(ARCH)
else
SRCDIR:=$(TARGET)
LIBDIR:=$(TARGET)
ARCHDIR:=bin/genfiles/$(TARGET)
UPDATEARCH:=$(TARGET)
endif
update: 
	@if [ "$(UPDATEARCH)" = "$(PATCH_ARCH)" ]; then\
	  cd bin/genfiles; echo "UPDATING REFERENCE ARCH $(UPDATEARCH)";\
	  for arch in $(ALL_ARCHS); do\
	    ./extract_patch $(PATCH_ARCH) $$arch;\
	  done; cd ../..;\
	else\
	  cd bin/genfiles; ./extract_patch $(PATCH_ARCH) $(UPDATEARCH);\
	  cd ../..;\
	fi
	@for i in $(UPDATE_SRCS); do (cmp -s $(SRCDIR)/$$i $(ARCHDIR)/src/$$i || (echo UPDATING $(SRCDIR)/$$i; cp $(SRCDIR)/$$i $(ARCHDIR)/src/$$i)) done
	@for i in $(C_LIBS); do (cmp -s $(LIBDIR)/$$i $(ARCHDIR)/lib/$$i || (echo UPDATING $(LIBDIR)/$$i; cp $(LIBDIR)/$$i $(ARCHDIR)/lib/$$i)) done
	@cmp -s $(LIBDIR)/nogc.c $(ARCHDIR)/lib/nogc.c || (echo UPDATING $(LIBDIR)/nogc.c; cp $(LIBDIR)/nogc.c $(ARCHDIR)/lib/nogc.c)
	@cmp -s $(LIBDIR)/$(C_RUNTIME) $(ARCHDIR)/lib/$(C_RUNTIME) || (echo UPDATING $(LIBDIR)/$(C_RUNTIME); cp $(LIBDIR)/$(C_RUNTIME) $(ARCHDIR)/lib/$(C_RUNTIME))
	@cmp -s $(LIBDIR)/precore_c.h $(ARCHDIR)/lib/precore_c.h || (echo UPDATING $(LIBDIR)/precore_c.h; cp $(LIBDIR)/precore_c.h $(ARCHDIR)/lib/precore_c.h)
ifndef TARGET
	@cmp -s $(SRCDIR)/cycdoc.c $(ARCHDIR)/src/cycdoc.c || (echo UPDATING $(SRCDIR)/cycdoc.c; cp $(SRCDIR)/cycdoc.c $(ARCHDIR)/src/cycdoc.c)
	@cmp -s $(SRCDIR)/buildlib.c $(ARCHDIR)/src/buildlib.c || (echo UPDATING $(SRCDIR)/buildlib.c; cp $(SRCDIR)/buildlib.c $(ARCHDIR)/src/buildlib.c)
	@for i in $(CYCLONE_H); do (cmp -s lib/$$i include/$$i || (echo UPDATING lib/$$i; cp lib/$$i include/$$i)) done
	@(cd lib; for i in arch/*.h; do (cmp -s $$i ../include/$$i || (echo UPDATING lib/$$i; cp $$i ../include/$$i)) done)
	@cmp -s lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h || (echo UPDATING cyc-lib/include/cyc_include.h; cp lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h)
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
	  if [ ! -d "$(PATCH_ARCH)" ]; then mkdir $(PATCH_ARCH); fi;\
          $(MAKE) -C lib TARGET=$(PATCH_ARCH) $(PATCH_ARCH);\
	  $(MAKE) -C src TARGET=$(PATCH_ARCH) $(PATCH_ARCH);\
	  $(MAKE) update TARGET=$(PATCH_ARCH);\
	else\
	  $(MAKE) cyclone_src;\
	  $(MAKE) update;\
	fi;
	@for arch in $(ALL_ARCHS); do\
	  if [ "$$arch" != "$(PATCH_ARCH)" ]; then\
	    if [ "$$arch" != "$(ARCH)" ]; then\
	      if [ ! -d "$$arch" ]; then mkdir $$arch; fi;\
              $(MAKE) -C lib TARGET=$$arch $$arch;\
	      $(MAKE) -C src TARGET=$$arch $$arch;\
	      $(MAKE) update TARGET=$$arch;\
	    else\
	      $(MAKE) cyclone_src;\
	      $(MAKE) update;\
	    fi;\
	  fi;\
	done

# only update development architectures
update_devel_archs:
	$(MAKE) update_all_archs ALL_ARCHS="$(DEVEL_ARCHS)"

# affected by CYCC and OUT_PREFIX
test:
	$(MAKE) -C tests

clean_src:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib

# affected by OUT_PREFIX
clean_src_prefix:
	$(MAKE) clean_prefix -C src 
	$(MAKE) clean_prefix -C lib 

# To do: using OUT_PREFIX for other targets is a wasteful kludge.
# To do: cross-compiler cleaning doesn't remove copies of no_gc.c and 
#        runtime_cyc.c.
clean_nogc:
	$(MAKE) clean -C tools/bison
	$(MAKE) clean -C tools/cyclex
	$(MAKE) clean -C tools/aprof
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
	$(MAKE) clean -C tests
	@for arch in $(ALL_ARCHS); do\
	    if [ -d "$$arch" ]; then\
	    $(MAKE) clean_prefix -C src OUT_PREFIX=../$$arch/;\
	    $(MAKE) clean_prefix -C lib OUT_PREFIX=../$$arch/;\
	    fi;\
	done
	$(RM) bin/cyc-lib/libcyc.a bin/cyc-lib/libcyc_a.a
	$(MAKE) clean -C bin/genfiles
	$(MAKE) clean -C tests
	$(MAKE) clean -C doc
	$(RM) bin/cyclone bin/cyclone.exe 
	$(RM) bin/cycdoc bin/cycdoc.exe 
	$(RM) bin/buildlib bin/buildlib.exe 
	$(RM) bin/cycbison bin/cycbison.exe 
	$(RM) bin/cyclex bin/cyclex.exe
	$(RM) bin/aprof bin/aprof.exe
	$(RM) *~ doc/*~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
# 	$(RM) bin/cyc-lib/gc.a bin/gc_pg.a 
# 	$(RM) bin/cyc-lib/nogc.a bin/cyc-lib/nogc_a.a
	$(RM) bin/cyc-lib/*.a
