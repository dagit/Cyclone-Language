
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

# These are passed to src and lib to control bootstrapping.  Override them
# on the command-line (using a path that will be interpreted in src and lib)
CYCC=../bin/$(CYCCOMP) # The .. is b/c this variable is used in lib and src
OUT_PREFIX=

all: bin/cyc-lib/gc.a cyclone tools

install: all

# This target builds off the C files in bin/genfiles
cyclone:
	$(MAKE) install -C bin/genfiles

tools:
	$(MAKE) install -C tools/cycbison
	$(MAKE) install -C tools/cyclex
#	$(MAKE) install -C tools/cycocamllex
.PHONY: tools

bin/cyc-lib/gc.a:
	$(MAKE) -C gc gc.a CC=gcc
	ln gc/gc.a $@

# These targets build off the Cyclone source files, 
# but do not replace anything in bin
cyclone_src: lib_src
	$(MAKE) -C src CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

lib_src:
	$(MAKE) -C lib CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

# Allocation profiler and its special version of the Cyclone library
aprof:
	$(MAKE) install_a -C bin/genfiles
	$(MAKE) install -C tools/aprof

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
diff: cyclone_src
	for i in $(C_SRCS); do (diff bin/genfiles/src/$$i src/$$i) done
	for i in $(C_LIBS); do (diff bin/genfiles/lib/$$i lib/$$i) done
	for i in $(CYCLONE_H); do (diff include/$$i lib/$$i) done
	diff bin/genfiles/lib/$(C_RUNTIME) lib/$(C_RUNTIME)
	diff bin/genfiles/lib/precore_c.h lib/precore_c.h
	diff bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h
	diff bin/genfiles/lib/nogc.c lib/nogc.c

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
cmp: 
	@for i in $(C_SRCS); do (cmp -s bin/genfiles/src/$$i src/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX src/$$i CHANGED) done
	@for i in $(C_LIBS); do (cmp -s bin/genfiles/lib/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@for i in $(CYCLONE_H); do (cmp -s include/$$i lib/$$i || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$$i CHANGED) done
	@cmp -s bin/genfiles/lib/$(C_RUNTIME) lib/$(C_RUNTIME) || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/$(C_RUNTIME) CHANGED
	@cmp -s bin/genfiles/lib/precore_c.h lib/precore_c.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/precore_c.h CHANGED
	@cmp -s bin/cyc-lib/include/cyc_include.h lib/include/cyc_include.h || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX cyc-lib/include/cyc_include.h CHANGED
	@cmp -s bin/genfiles/lib/nogc.c lib/nogc.c || echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX lib/nogc.c CHANGED

# This target updates what is in bin/genfiles and include.
# It would be "dangerous"
# to invoke this target with "make -k" if we did not have version control.
# I intend to put a "warning" waiting for user response in (and remind
# to add files to Makefile.inc).
# TJIM: modified to update only changed files.  This speeds up cvs commit
# over a slow link.
update: cyclone_src
	@for i in $(C_SRCS); do (cmp -s src/$$i bin/genfiles/src/$$i || (echo UPDATING src/$$i; cp src/$$i bin/genfiles/src/$$i)) done
	@for i in $(C_LIBS); do (cmp -s lib/$$i bin/genfiles/lib/$$i || (echo UPDATING lib/$$i; cp lib/$$i bin/genfiles/lib/$$i)) done
	@for i in $(CYCLONE_H); do (cmp -s lib/$$i include/$$i || (echo UPDATING lib/$$i; cp lib/$$i include/$$i)) done
	@cmp -s lib/$(C_RUNTIME) bin/genfiles/lib/$(C_RUNTIME) || (echo UPDATING lib/$(C_RUNTIME); cp lib/$(C_RUNTIME) bin/genfiles/lib/$(C_RUNTIME))
	@cmp -s lib/precore_c.h bin/genfiles/lib/precore_c.h || (echo UPDATING lib/precore_c.h; cp lib/precore_c.h bin/genfiles/lib/precore_c.h)
	@cmp -s lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h || (echo UPDATING cyc-lib/include/cyc_include.h; cp lib/include/cyc_include.h bin/cyc-lib/include/cyc_include.h)
	@cmp -s lib/nogc.c bin/genfiles/lib/nogc.c || (echo UPDATING lib/nogc.c; cp lib/nogc.c bin/genfiles/lib/nogc.c)

test:
	$(MAKE) -C tests CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

clean_src:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib

clean_src_prefix:
	$(MAKE) clean_prefix -C src OUT_PREFIX=$(OUT_PREFIX)
	$(MAKE) clean_prefix -C lib OUT_PREFIX=$(OUT_PREFIX)

clean_nogc:
	$(MAKE) clean -C tools/cycbison
	$(MAKE) clean -C tools/cyclex
	$(MAKE) clean -C tools/aprof
#	$(MAKE) clean -C tools/cycocamllex
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
	$(MAKE) clean -C bin/genfiles
	$(MAKE) clean -C tests
	$(RM) bin/cyclone bin/cyclone.exe 
	$(RM) bin/cyc-lib/$(CYCLIB)
	$(RM) bin/cycbison bin/cycbison.exe 
	$(RM) bin/cyclex bin/cyclex.exe
	$(RM) bin/aprof bin/aprof.exe
	$(RM) bin/cyc-lib/libcyc_a.a bin/cyc-lib/nogc_a.a
	$(RM) *~ doc/*~ amon.out

clean: clean_nogc
	$(MAKE) clean -C gc
	$(RM) gc/*.exe gc/base_lib gc/*.obj gc/gc.lib
	$(RM) bin/cyc-lib/gc.a bin/gc_pg.a bin/cyc-lib/nogc.a
