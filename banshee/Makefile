# Copyright (c) 2000-2004
#      The Regents of the University of California.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

BANSHEE_VERSION := banshee-1.0
DISTRIB_ROOT := /tmp
DISTRIB_DIR := $(DISTRIB_ROOT)/$(BANSHEE_VERSION)

.PHONY: banshee libcompat codegen engine tests andersen steensgaard \
	lambda dyckcfl docs points-to install perf ibanshee java distrib \
#	rca rlib

all: banshee ibanshee tests points-to 

banshee: libcompat codegen engine #rlib

libcompat: 
	$(MAKE) CC=$(CC) -C libcompat

#rlib:
#	$(MAKE) CC=$(CC) -C rlib

codegen: 
	$(MAKE) CC=$(CC) -C codegen

engine:
	$(MAKE) CC=$(CC) -C engine 

rca: banshee
	$(MAKE) -C rca

andersen: banshee
	$(MAKE) -C andersen

steensgaard: banshee
	$(MAKE) -C steensgaard

points-to: andersen steensgaard
	$(MAKE) -C cparser

ibanshee: banshee
	$(MAKE) -C ibanshee

lambda: banshee
	$(MAKE) -C lambda

dyckcfl: banshee
	$(MAKE) -C dyckcfl

java: banshee dyckcfl
	$(MAKE) -C java

tests: banshee andersen steensgaard lambda dyckcfl # rca
	$(MAKE) -C tests

docs: 
	$(MAKE) -C docs

check: 
	$(MAKE) -C tests check
	$(MAKE) -C tests -f pta_test.mk
ifndef NO_BANSHEE_ROLLBACK
	$(MAKE) -C tests -f pta_test.mk pta-bt-regr
#	$(MAKE) -C tests -f pta_test.mk pta-persist-regr
endif
#ifdef RLIB
	$(MAKE) -C tests -f pta_test.mk pta-rpersist-regr
#endif
	$(MAKE) -C tests -f ibanshee_test.mk
ifndef NO_BANSHEE_ROLLBACK
	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-bt
#	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-persist
endif
#ifdef RLIB
	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-rpersist
#endif
	@echo "SUCCESS: All check targets passed."

rpersist_check:
	$(MAKE) -C tests check/region-persist-test.exe
	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-rpersist
	$(MAKE) -C tests -f pta_test.mk pta-rpersist-regr
	@echo "SUCCESS: All region persistence check targets passed."

perf:
	$(MAKE) -C tests -f pta_perf.mk

install:
	@echo "banshee does not have an install target."

clean:
	$(MAKE) -C libcompat clean
#	$(MAKE) -C rlib clean
	$(MAKE) -C codegen clean
	$(MAKE) -C engine clean
	$(MAKE) -C rca clean
	$(MAKE) -C andersen clean
	$(MAKE) -C steensgaard clean
	$(MAKE) -C cparser clean
	$(MAKE) -C lambda clean
	$(MAKE) -C dyckcfl clean
	$(MAKE) -C tests clean
	$(MAKE) -C docs clean
	$(MAKE) -C tests -f pta_test.mk clean
	$(MAKE) -C tests -f ibanshee_test.mk clean
	$(MAKE) -C ibanshee clean
	$(MAKE) -C java clean
	rm -rf *~ data statics offsets extras banshee*.tar.gz

distrib: clean docs
	rm -rf $(DISTRIB_DIR)
	mkdir $(DISTRIB_DIR)
# make a documentation directory and copy the documentation over
	mkdir $(DISTRIB_DIR)/docs
	cp docs/Makefile $(DISTRIB_DIR)/docs/
	cp docs/banshee-manual.pdf $(DISTRIB_DIR)/docs/
	cp docs/pldi04-kodumal.pdf $(DISTRIB_DIR)/docs/
	cp docs/sas05-kodumal.pdf $(DISTRIB_DIR)/docs/
# make an engine directory and copy the sources over
	mkdir $(DISTRIB_DIR)/engine
	cp engine/Makefile $(DISTRIB_DIR)/engine/
	cp engine/*.c $(DISTRIB_DIR)/engine/
	cp engine/*.h $(DISTRIB_DIR)/engine/
# make a bin directory and copy the sources over
	mkdir $(DISTRIB_DIR)/bin
	cp bin/build_funptr_table.py $(DISTRIB_DIR)/bin/
# make an andersen directory and copy the sources over
	mkdir $(DISTRIB_DIR)/andersen
	cp andersen/Makefile $(DISTRIB_DIR)/andersen/
	cp andersen/*.h $(DISTRIB_DIR)/andersen/
	cp andersen/*.bsp $(DISTRIB_DIR)/andersen/
	cp andersen/*.c $(DISTRIB_DIR)/andersen/
# make a codegen directory and copy the sources over
	mkdir $(DISTRIB_DIR)/codegen
	cp -r codegen/* $(DISTRIB_DIR)/codegen
	rm -rf $(DISTRIB_DIR)/codegen/CVS
# make a cparser directory and copy the sources over (make an mobj dir, too)
	mkdir $(DISTRIB_DIR)/cparser
	cp -r cparser/* $(DISTRIB_DIR)/cparser
	rm -rf $(DISTRIB_DIR)/cparser/CVS
	rm -rf $(DISTRIB_DIR)/cparser/mobj/CVS
	rm -rf $(DISTRIB_DIR)/cparser/robj/CVS
	rm -rf $(DISTRIB_DIR)/cparser/obj/CVS
# make a dyckcfl directory and copy the sources over
	mkdir $(DISTRIB_DIR)/dyckcfl
	cp dyckcfl/*.c $(DISTRIB_DIR)/dyckcfl/
	cp dyckcfl/*.h $(DISTRIB_DIR)/dyckcfl/
	cp dyckcfl/Makefile $(DISTRIB_DIR)/dyckcfl/
# make an ibanshee directory and copy the sources over
	mkdir $(DISTRIB_DIR)/ibanshee
	cp ibanshee/parser.y $(DISTRIB_DIR)/ibanshee/
	cp ibanshee/lexer.lex $(DISTRIB_DIR)/ibanshee/
	cp ibanshee/Makefile $(DISTRIB_DIR)/ibanshee/
# make a java directory and copy the sources over
	mkdir $(DISTRIB_DIR)/java
	cp java/Makefile $(DISTRIB_DIR)/java/
# make an rca directory and copy the sources over
	mkdir $(DISTRIB_DIR)/rca
	cp rca/Makefile $(DISTRIB_DIR)/rca/
# make a lambda directory and copy the sources over
	mkdir $(DISTRIB_DIR)/lambda
	cp lambda/lambda.bsp $(DISTRIB_DIR)/lambda/
	cp lambda/Makefile $(DISTRIB_DIR)/lambda/
# make a libcompat directory and copy the sources over
	mkdir $(DISTRIB_DIR)/libcompat
	cp -r libcompat/ $(DISTRIB_DIR)/libcompat/
	rm -rf $(DISTRIB_DIR)/libcompat/CVS
# make an rlib directory and copy the sources over
#	mkdir $(DISTRIB_DIR)/rlib
#	cp -r rlib/ $(DISTRIB_DIR)/rlib/
#	rm -rf $(DISTRIB_DIR)/rlib/CVS
#	rm -rf $(DISTRIB_DIR)/rlib/test/
# make a steensgaard directory and copy the sources over
	mkdir $(DISTRIB_DIR)/steensgaard
	cp steensgaard/*.bsp $(DISTRIB_DIR)/steensgaard/
	cp steensgaard/*.c $(DISTRIB_DIR)/steensgaard/
	cp steensgaard/*.h $(DISTRIB_DIR)/steensgaard/
	cp steensgaard/Makefile $(DISTRIB_DIR)/steensgaard/
# make a tests directory and copy the sources over
	mkdir $(DISTRIB_DIR)/tests
	mkdir $(DISTRIB_DIR)/tests/test.cor
	mkdir $(DISTRIB_DIR)/tests/test.ibc
	mkdir $(DISTRIB_DIR)/tests/test.ibc.cor
	mkdir $(DISTRIB_DIR)/tests/test.programs
	cp tests/*.mk $(DISTRIB_DIR)/tests/
	cp tests/*.py $(DISTRIB_DIR)/tests/
	cp tests/*.c $(DISTRIB_DIR)/tests/
	cp tests/Makefile $(DISTRIB_DIR)/tests/
	cp tests/test.cor/*.cor $(DISTRIB_DIR)/tests/test.cor/
	cp -r tests/test.ibc/ $(DISTRIB_DIR)/tests/test.ibc/
	rm -rf $(DISTRIB_DIR)/tests/test.ibc/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc/bt/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc/persist/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc/rpersist/CVS
	cp -r tests/test.ibc.cor/ $(DISTRIB_DIR)/tests/test.ibc.cor/
	rm -rf $(DISTRIB_DIR)/tests/test.ibc.cor/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc.cor/bt/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc.cor/persist/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.ibc.cor/rpersist/CVS
	cp -r tests/test.programs/* $(DISTRIB_DIR)/tests/test.programs/
	rm -rf $(DISTRIB_DIR)/tests/test.programs/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.programs/cqual.preproc/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.programs/espresso.preproc/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.programs/li.preproc/CVS
	rm -rf $(DISTRIB_DIR)/tests/test.programs/ML-typecheck.preproc/CVS
# copy Makefile, README, and COPYRIGHT
	cp Makefile $(DISTRIB_DIR)/
	cp README $(DISTRIB_DIR)/
	cp COPYRIGHT $(DISTRIB_DIR)/
#tar and zip the distrib directory
	cd $(DISTRIB_ROOT); tar cfz $(BANSHEE_VERSION).tar.gz $(BANSHEE_VERSION)
	mv $(DISTRIB_ROOT)/$(BANSHEE_VERSION).tar.gz . 
