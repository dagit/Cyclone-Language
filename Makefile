
# The following instructions assume you have not added any files to
# src or lib.  If you have, you MUST MODIFY Makefile.inc in the
# obvious manner.

# To update to a new version of the compiler, here's the boostrap sequence
#   1. make all
#   2. make cyclone_src
#   3. make update
#   4. make cyclone
# It is strongly advised that you precede step 3 with the boostrap-checking
# facilities provided by CYCC and OUT_PREFIX.

# To use a new version of the compiler without nuking the C files used to
# bootstrap:
#   1. make all
#   2. make cyclone_src
# There is now a src/cyclone.exe that you can use/move as you wish.

include Makefile.inc

ifndef ARCH
$(error "Must have ARCH variable defined to properly compile")
else
ifndef TARGET
TARGET := $(ARCH)
endif
endif

CYC_BIN_PATH := $(CYCDIR)/bin
CYC_LIB_PATH := $(CYCDIR)/bin/cyc-lib/
CYC_INC_PATH := $(CYCDIR)/lib

# These are passed to src and lib to control bootstrapping.  Override them
# on the command-line (using a path that will be interpreted in src and lib)
CYCC=$(CYC_BIN_PATH)/$(CYCCOMP) 
OUT_PREFIX=

build: $(CYC_LIB_PATH)/gc.a cyclone tools

# This target builds off the C files in bin/genfiles
cyclone:
	$(MAKE) install -C bin/genfiles/$(ARCH)

tools:
	$(MAKE) install -C tools/bison
	$(MAKE) install -C tools/cyclex
#	$(MAKE) install -C tools/cycocamllex
.PHONY: tools

$(CYC_LIB_PATH)/gc.a:
	$(MAKE) -C gc gc.a CC=gcc
	ln gc/gc.a $@

# After building all of the source, install it in the user-defined 
# directories.  Also, keep a record of what was copied for later
# uninstall.
install: build inc_install lib_install bin_install
uninstall: inc_uninstall lib_uninstall bin_uninstall

ifdef INC_INSTALL
inc_install:
	config/cyc_install include/* $(INC_INSTALL)
inc_uninstall:
	config/cyc_install -u $(INC_INSTALL)
else
inc_install:
	@(echo "no include directory specified"; exit 1)
inc_uninstall:
	@(echo "no include directory specified"; exit 1)
endif

ifdef BIN_INSTALL
bin_install:
	config/cyc_install bin/cyclone$(EXE) bin/cycbison$(EXE) bin/cyclex$(EXE) $(BIN_INSTALL)
bin_uninstall:
	config/cyc_install -u $(BIN_INSTALL)
else
bin_install:
	@(echo "no bin directory specified"; exit 1)
bin_uninstall:
	@(echo "no bin directory specified"; exit 1)
endif

ifdef LIB_INSTALL
lib_install:
	config/cyc_install bin/cyc-lib/* $(LIB_INSTALL)
lib_uninstall:
	config/cyc_install -u $(LIB_INSTALL)
else
lib_install:
	@(echo "no lib directory specified"; exit 1)
lib_uninstall:
	@(echo "no lib directory specified"; exit 1)
endif

# These targets build off the Cyclone source files, 
# but do not replace anything in bin
cyclone_src: lib_src
	$(MAKE) -C src CYCC=$(CYCC) TARGET=$(TARGET) OUT_PREFIX=$(OUT_PREFIX)

lib_src:
	$(MAKE) -C lib CYCC=$(CYCC) TARGET=$(TARGET) OUT_PREFIX=$(OUT_PREFIX)

# to make just the source files (no linking or .o), for cross-compile
cyclone_srconly: lib_srconly
	$(MAKE) -C src src CYCC=$(CYCC) TARGET=$(TARGET) OUT_PREFIX=$(OUT_PREFIX)

lib_srconly:
	$(MAKE) -C lib src CYCC=$(CYCC) TARGET=$(TARGET) OUT_PREFIX=$(OUT_PREFIX)

# Allocation profiler and its special version of the Cyclone library
aprof:
	$(MAKE) install_a -C bin/genfiles/$(ARCH)
	$(MAKE) install -C tools/aprof

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
diff: cyclone_src
	for i in $(C_SRCS); do (diff bin/genfiles/$(TARGET)/src/$$i src/$$i) done
	for i in $(C_LIBS); do (diff bin/genfiles/$(TARGET)/lib/$$i lib/$$i) done
	for i in $(CYCLONE_H); do (diff include/$$i lib/$$i) done
	diff bin/genfiles/$(TARGET)/lib/$(C_RUNTIME) lib/$(C_RUNTIME)
	diff bin/genfiles/$(TARGET)/lib/precore_c.h lib/precore_c.h
	diff bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h
	diff bin/genfiles/$(TARGET)/lib/nogc.c lib/nogc.c

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
cmp: 
	@for i in $(C_SRCS); do (cmp -s bin/genfiles/$(TARGET)/src/$$i src/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX src/$$i CHANGED) done
	@for i in $(C_LIBS); do (cmp -s bin/genfiles/$(TARGET)/lib/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@for i in $(CYCLONE_H); do (cmp -s include/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@cmp -s bin/genfiles/$(TARGET)/lib/$(C_RUNTIME) lib/$(C_RUNTIME) || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$(C_RUNTIME) CHANGED
	@cmp -s bin/genfiles/$(TARGET)/lib/precore_c.h lib/precore_c.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/precore_c.h CHANGED
	@cmp -s bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX cyc-lib/include/cyc_include.h CHANGED
	@cmp -s bin/genfiles/$(TARGET)/lib/nogc.c lib/nogc.c || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/nogc.c CHANGED

# This target updates what is in bin/genfiles and include.
# It would be "dangerous"
# to invoke this target with "make -k" if we did not have version control.
# I intend to put a "warning" waiting for user response in (and remind
# to add files to Makefile.inc).
# TJIM: modified to update only changed files.  This speeds up cvs commit
# over a slow link.
# MWH: Added a check to make sure that the .c files match the target we are
# installing into
update: cyclone_src
	@if [ ! -f "src/.target" -o "`cat src/.target`" != "$(TARGET)" ]; then echo "Attempting to update $(TARGET) with files built for `cat src/.target`"; exit 1; fi
	@for i in $(C_SRCS); do (cmp -s src/$$i bin/genfiles/$(TARGET)/src/$$i || (echo UPDATING src/$$i; cp src/$$i bin/genfiles/$(TARGET)/src/$$i)) done
	@if [ ! -f "lib/.target" -o "`cat lib/.target`" != "$(TARGET)" ]; then echo "Attempting to update $(TARGET) with files built for `cat lib/.target`"; exit 1; fi
	@for i in $(C_LIBS); do (cmp -s lib/$$i bin/genfiles/$(TARGET)/lib/$$i || (echo UPDATING lib/$$i; cp lib/$$i bin/genfiles/$(TARGET)/lib/$$i)) done
	@for i in $(CYCLONE_H); do (cmp -s lib/$$i include/$$i || (echo UPDATING lib/$$i; cp lib/$$i include/$$i)) done
	@(cd lib; for i in arch/*.h; do (cmp -s $$i ../include/$$i || (echo UPDATING lib/$$i; cp $$i ../include/$$i)) done)
	@cmp -s lib/$(C_RUNTIME) bin/genfiles/$(TARGET)/lib/$(C_RUNTIME) || (echo UPDATING lib/$(C_RUNTIME); cp lib/$(C_RUNTIME) bin/genfiles/$(TARGET)/lib/$(C_RUNTIME))
	@cmp -s lib/precore_c.h bin/genfiles/$(TARGET)/lib/precore_c.h || (echo UPDATING lib/precore_c.h; cp lib/precore_c.h bin/genfiles/$(TARGET)/lib/precore_c.h)
	@cmp -s lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h || (echo UPDATING cyc-lib/include/cyc_include.h; cp lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h)
	@cmp -s lib/nogc.c bin/genfiles/$(TARGET)/lib/nogc.c || (echo UPDATING lib/nogc.c; cp lib/nogc.c bin/genfiles/$(TARGET)/lib/nogc.c)

# This will destroy any existing compiled files in lib and src
#   and then compile and install for all supported architectures
update_all_archs:
	@for arch in $(ALL_ARCHS); do $(MAKE) clean_src update TARGET=$$arch; done

test:
	$(MAKE) -C tests CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

clean_src:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib

clean_src_prefix:
	$(MAKE) clean_prefix -C src OUT_PREFIX=$(OUT_PREFIX)
	$(MAKE) clean_prefix -C lib OUT_PREFIX=$(OUT_PREFIX)

clean_nogc:
	$(MAKE) clean -C tools/bison
	$(MAKE) clean -C tools/cyclex
	$(MAKE) clean -C tools/aprof
#	$(MAKE) clean -C tools/cycocamllex
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
	$(RM) bin/cyc-lib/libcyc.a bin/cyc-lib/libcyc_a.a
	$(MAKE) clean -C bin/genfiles
	$(MAKE) clean -C tests
	$(RM) bin/cyclone bin/cyclone.exe 
	$(RM) bin/cycbison bin/cycbison.exe 
	$(RM) bin/cyclex bin/cyclex.exe
	$(RM) bin/aprof bin/aprof.exe
	$(RM) *~ doc/*~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
	$(RM) bin/cyc-lib/gc.a bin/gc_pg.a 
	$(RM) bin/cyc-lib/nogc.a bin/cyc-lib/nogc_a.a
