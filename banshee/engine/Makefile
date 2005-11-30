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

ifdef NO_HASH_BOUNDS
FILTEREDOBJS = hash_bounds.o
else
FILTEREDOBJS = bounds.o
endif

FILTEREDOBJS += malloc.o
NONSPEC_OBJ = nonspec.o

ifndef RLIB
region_dir = ../libcompat
else
region_dir = ../rlib
endif

CC = gcc

CFLAGS = -I. -I$(region_dir) -Ddeletes= -Dtraditional= -Dsameregion= \
	 -Dparentptr= -Wall -Werror -fno-strict-aliasing -fno-common 

ifndef NO_BANSHEE_ROLLBACK
CFLAGS := -DBANSHEE_ROLLBACK ${CFLAGS}
endif

#HDRS = $(wildcard *.h)

HDRS = array.h banshee.h banshee_persist_kinds.h banshee_region_persist_kinds.h bool.h bounds.h buffer.h compiler.h cstring.h flow-var.h flowrow-sort.h hash.h hashset.h jcollection.h linkage.h list.h nonspec.h persist.h setif-sort.h setif-var.h setst-sort.h setst-var.h stamp.h term-sort.h term-var.h termhash.h ufind.h usage.h utils.h

COBJS = array.c banshee.c banshee_persist_kinds.c banshee_region_persist_kinds.c bounds.c buffer.c flow-var.c flowrow-sort.c hash.c hash_bounds.c hashset.c jcollection.c list.c malloc.c nonspec.c persist.c setif-sort.c setif-var.c setst-sort.c setst-var.c stamp.c term-sort.c term-var.c termhash.c ufind.c usage.c utils.c

NFOBJS = $(patsubst %.c,%.o,$(COBJS))

AOBJS = $(filter-out $(FILTEREDOBJS), $(NFOBJS))

OBJS = $(filter-out $(NONSPEC_OBJ), $(AOBJS))

NSOBJS = $(patsubst %.o,%.ns.o,$(AOBJS))

#TAGFILES = $(wildcard *.c) $(wildcard *.h)

TARGETS = libengine.a libnsengine.a 

ifdef DEBUG
CFLAGS := -pg -g ${CFLAGS}
else
CFLAGS := -DNDEBUG -O3 ${CFLAGS}
endif

all: $(TARGETS) 

libengine.a: $(OBJS)
	@echo $(OBJS)
	../bin/build_funptr_table.py $(OBJS) > fn_ptrs.c
	$(CC) -c $(CFLAGS) fn_ptrs.c -o fn_ptrs.o
	$(AR) rcs $@ $^ fn_ptrs.o

libnsengine.a: $(NSOBJS) 
	@echo $(NSOBJS)
	../bin/build_funptr_table.py $(NSOBJS) > fn_ptrs.ns.c
	$(CC) -c $(CFLAGS) fn_ptrs.ns.c -o fn_ptrs.o
	$(AR) rcs $@ $^ fn_ptrs.o

nonspec.o: nonspec.c nonspec.h
	$(CC) -c $(CFLAGS) -DNONSPEC $(CPPFLAGS) $< -o $@

%.ns.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) -DNONSPEC $(CPPFLAGS) $< -o $@

%.o : %.c $(HDRS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean

#TAGS: $(TAGFILES)
#	etags ${TAGFILES}

clean:
	-rm -f *.o $(TARGETS) *~ fn_ptrs.ns.c fn_ptrs.c




