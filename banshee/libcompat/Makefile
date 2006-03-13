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

CC=gcc

ifdef DEBUG_RALLOC
CFLAGS = -g -pg -Wall -Ddeletes="" -DDEBUG_RALLOC -DNDEBUG -DNMEMDEBUG  -Werror -fno-common
else
CFLAGS = -Wall -Ddeletes="" -DNMEMDEBUG -DNDEBUG -Werror -fno-common
endif

TAGFILES = $(wildcard *.c) $(wildcard *.h)

TARGETS = libregions.a 

all: $(TARGETS) 

regions.o: serialize.c regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -o regions.o regions.c

profile.o: 
	gcc -c $(CFLAGS) -o profile.o profile.c

libregions.a: regions.o profile.o
	$(AR) rcs $@ $^

TAGS: $(TAGFILES)
	etags $(TAGFILES)

.PHONY: clean

clean:
	rm -f $(TARGETS) regions.o profile.o *~
