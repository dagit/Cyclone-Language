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

PARSER_DIR = ../cparser

.PHONY: pta-check
pta-check: pta-regr

PTA_DIR := ./test.programs
COR_DIR := ./test.cor

PTA_LARGE := 
PTA_LARGE += ML-typecheck.preproc
PTA_LARGE += li.preproc

PTA_BT :=
PTA_BT += ML-typecheck.preproc
PTA_BT += li.preproc

PTA_PERSIST :=
PTA_PERSIST += ML-typecheck.preproc
PTA_PERSIST += li.preproc

PTA_RPERSIST :=
PTA_RPERSIST += ML-typecheck.preproc

PTA_EXEC := $(PARSER_DIR)/parser.exe

PTA_NS_EXEC := $(PARSER_DIR)/parser_ns.exe

.PHONY: pta-regr pta-bt-regr
pta-regr:  $(PARSER_DIR)/parser.exe pta-regr/clean \
           $(patsubst %,pta-large/%,$(PTA_LARGE)) pta-done

pta-bt-regr: $(PARSER_DIR)/parser.exe \
	   $(patsubst %,pta-bt/%,$(PTA_BT)) pta-bt-done

pta-persist-regr: $(PARSER_DIR)/parser_ns.exe \
	   $(patsubst %,pta-persist/%,$(PTA_PERSIST)) pta-persist-done

pta-rpersist-regr: $(PARSER_DIR)/parser_ns.exe \
	   $(patsubst %,pta-rpersist/%,$(PTA_RPERSIST)) pta-rpersist-done

pta-regr/clean:
	rm -f $(PTA_DIR)/*.out
	rm -f andersen.out

.PHONY: pta-done pta-bt-done pta-persist-done pta-rpersist-done

pta-done:; @echo "PTA tests pass"

pta-bt-done:; @echo "PTA backtracking tests pass"

pta-persist-done:; @echo "PTA persistence tests pass"

pta-rpersist-done:; @echo "PTA region persistence tests pass"

pta-large/%:
	$(PTA_EXEC) $(PTA_DIR)/$*/*.c 2> /dev/null | grep "Number of things pointed to" > $(PTA_DIR)/$*.out
	diff $(COR_DIR)/$*.cor $(PTA_DIR)/$*.out

pta-bt/%:
	./pta-bt-test.py 7 $(PTA_DIR)/$*/*.c

pta-persist/%:
	$(PTA_NS_EXEC) -fserialize-constraints $(PTA_DIR)/$*/*.c 2>/dev/null > /dev/null
	$(PTA_NS_EXEC) -fdeserialize-constraints 2>/dev/null | grep "Number of things pointed to" > $(PTA_DIR)/$*_persist.out
	diff $(COR_DIR)/$*.cor $(PTA_DIR)/$*_persist.out

pta-rpersist/%:
	./andersen-rpersist-test.py $(PTA_DIR)/$*/*.c

$(PARSER_DIR)/parser.exe: 
	$(MAKE) -C ../ points-to	

clean: pta-regr/clean
