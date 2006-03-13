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

region_dir = ../libcompat

engine_dir = ../engine
dyckcfl_dir = ../dyckcfl
lambda_dir = ../lambda

DYCKCFL_EXTRA_OBJS = $(dyckcfl_dir)/dyckcfl.o $(dyckcfl_dir)/mr_dyckcfl.o
LAMBDA_EXTRA_OBJS = $(lambda_dir)/lambda.o


REGION_LIB = $(region_dir)/libregions.a
NSENGINE_LIB = $(engine_dir)/libnsengine.a
ENGINE_LIB = $(engine_dir)/libengine.a

CC = gcc

CFLAGS = -I$(engine_dir) -I$(region_dir) -Ddeletes= -Dtraditional= \
         -Dsameregion= -Dparentptr= -Wall -Werror

ifdef DEBUG
CFLAGS := -g -pg ${CFLAGS}
endif 

TAGFILES = $(wildcard *.c) $(wildcard *.h)

EXES = lambda-test.exe dyckcfl-random-test.exe \
       uf-bt-test.exe persist-test.exe dyckcfl-test.exe regexp-test.exe \
	region-persist-test.exe

TARGETS =  $(EXES) 

TARGETCHECK = $(patsubst %,check/%,$(EXES))

all: $(TARGETS) 

lambda-test.exe: lambda-test.c $(LAMBDA_EXTRA_OBJS) $(ENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -I$(lambda_dir) -o $@ $(LAMBDA_EXTRA_OBJS) $< -L$(region_dir) -L$(engine_dir) -lengine -lregions -lm

dyckcfl-test.exe: dyckcfl-test.c $(DYCKCFL_EXTRA_OBJS) $(NSENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -I$(dyckcfl_dir) -DNONSPEC -o $@ $(DYCKCFL_EXTRA_OBJS) $< -L$(region_dir) -L$(engine_dir) -lnsengine -lregions -lm 

dyckcfl-random-test.exe: dyckcfl-random-test.c $(DYCKCFL_EXTRA_OBJS) $(NSENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -I$(dyckcfl_dir) -DNONSPEC -o $@ $(DYCKCFL_EXTRA_OBJS) $< -L$(region_dir) -L$(engine_dir) -lnsengine -lregions -lm

uf-bt-test.exe: uf-bt-test.c $(ENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L$(region_dir) -L$(engine_dir) -lengine -lregions -lm

persist-test.exe: persist-test.c $(ENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L$(region_dir) -L$(engine_dir) -lengine -lregions -lm

regexp-test.exe: regexp-test.c $(NSENGINE_LIB) $(REGION_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L$(region_dir) -L$(engine_dir) -lnsengine -lregions -lm

region-persist-test.exe: region-persist-test.c $(ENGINE_LIB) $(PERSISTENT_REGION_OBJ)
	$(CC) $(CFLAGS) -o $@ $< -L$(region_dir) -L$(engine_dir) -lengine -lregions 

.PHONY: clean check 

$(LAMBDA_EXTRA_OBJS): 
	$(MAKE) -C ../lambda

$(DYCKCFL_EXTRA_OBJS):
	$(MAKE) -C ../dyckcfl

$(REGION_LIB):
	$(MAKE) -C $(region_dir)

$(NSENGINE_LIB):
	$(MAKE) -C ../engine libnsengine.a

$(ENGINE_LIB):
	$(MAKE) -C ../engine libengine.a

$(PERSISTENT_REGION_OBJ):
	$(MAKE) -C ../libcompat

TAGS: 
	etags $(TAGFILES)

check: $(TARGETS) $(TARGETCHECK)

check/%: 
	./$*
	@echo ./$* passed


clean:
	rm -f *.o $(TARGETS) *.exe *.dot *.out *~