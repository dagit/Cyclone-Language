
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

all: bin/gc.a cyclone tools 

install: all

# This target builds off the C files in bin/genfiles
cyclone:
	$(MAKE) install -C bin/genfiles

tools:
	$(MAKE) install -C tools/cycbison
	$(MAKE) install -C tools/cyclex
#	$(MAKE) install -C tools/cycocamllex
.PHONY: tools

bin/gc.a:
	$(MAKE) -C gc gc.a CC=gcc
	ln gc/gc.a $@

# These targets build off the Cyclone source files, 
# but do not replace anything in bin
cyclone_src: lib_src
	$(MAKE) -C src CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

lib_src:
	$(MAKE) -C lib CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

# This target compares the C files in bin/genfiles to those in src
# Lack of difference means running the update would have no real effect.
diff: cyclone_src
	for i in $(C_SRCS); do (diff bin/genfiles/src/$$i src/$$i) done
	for i in $(C_LIBS); do (diff bin/genfiles/lib/$$i lib/$$i) done
	diff bin/genfiles/lib/$(C_RUNTIME) lib/$(C_RUNTIME)
	diff bin/genfiles/lib/precore_c.h  lib/precore_c.h

# This target updates what is in bin/genfiles.  It would be "dangerous"
# to invoke this target with "make -k" if we did not have version control.
# I intend to put a "warning" waiting for user response in (and remind
# to add files to Makefile.inc).
update: cyclone_src
	for i in $(C_SRCS); do (cp src/$$i bin/genfiles/src/$$i) done
	for i in $(C_LIBS); do (cp lib/$$i bin/genfiles/lib/$$i) done
	cp lib/$(C_RUNTIME) bin/genfiles/lib/$(C_RUNTIME)
	cp lib/precore_c.h  bin/genfiles/lib/precore_c.h  

test:
	$(MAKE) -C tests CYCC=$(CYCC) OUT_PREFIX=$(OUT_PREFIX)

clean_src:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib

clean_src_prefix:
	$(MAKE) clean_prefix -C src OUT_PREFIX=$(OUT_PREFIX)
	$(MAKE) clean_prefix -C lib OUT_PREFIX=$(OUT_PREFIX)

clean:
	$(MAKE) clean -C tools/cycbison
	$(MAKE) clean -C tools/cyclex
#	$(MAKE) clean -C tools/cycocamllex
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
	$(MAKE) clean -C bin/genfiles
	$(MAKE) clean -C tests
	$(MAKE) clean -C gc
	rm -f gc/*.exe gc/base_lib
	rm -f bin/cyclone.exe 
	rm -f bin/$(CYCLIB)
	rm -f bin/$(RUNTIME).o
	rm -f bin/cycbison.exe 
	rm -f bin/cyclex.exe
	rm -f bin/gc.a
	rm -f *~ doc/*~
