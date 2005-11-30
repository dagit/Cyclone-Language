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

.PHONY: banshee libcompat codegen engine tests rca andersen steensgaard \
	lambda dyckcfl docs points-to install perf ibanshee rlib java

all: banshee ibanshee tests points-to 

banshee: libcompat codegen engine rlib

libcompat: 
	$(MAKE) -C libcompat

rlib:
	$(MAKE) -C rlib

codegen: 
	$(MAKE) -C codegen

engine:
	$(MAKE) -C engine

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

tests: banshee rca andersen steensgaard lambda dyckcfl
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
ifdef RLIB
	$(MAKE) -C tests -f pta_test.mk pta-rpersist-regr
endif
	$(MAKE) -C tests -f ibanshee_test.mk
ifndef NO_BANSHEE_ROLLBACK
	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-bt
#	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-persist
endif
ifdef RLIB
	$(MAKE) -C tests -f ibanshee_test.mk ibanshee-rpersist
endif
	@echo "SUCCESS: All check targets passed."

perf:
	$(MAKE) -C tests -f pta_perf.mk

install:
	@echo "banshee does not have an install target."

clean:
	$(MAKE) -C libcompat clean
	$(MAKE) -C rlib clean
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
	rm -rf *~ data statics offsets extras

