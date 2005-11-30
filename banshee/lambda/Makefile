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

ifndef RLIB
region_dir = ../libcompat
else
region_dir = ../rlib
endif

engine_dir = ../engine
banshee_bindir = ../bin

BANSHEE = $(banshee_bindir)/banshee.exe

LAMBDA = lambda

TARGETS =  $(LAMBDA).h $(LAMBDA).c $(LAMBDA).o 

TAGFILES = $(LAMBDA).h $(LAMBDA).c

CC = gcc

ifdef DEBUG
CFLAGS = -I$(engine_dir) -I$(region_dir) -Ddeletes= -Dtraditional= \
         -Dsameregion= -Dparentptr= -Wall -Werror -g -pg
else
CFLAGS = -I$(engine_dir) -I$(region_dir) -Ddeletes= -Dtraditional= \
         -Dsameregion= -Dparentptr= -Wall -Werror
endif


.PHONY: clean

all: $(TARGETS)

$(LAMBDA).c: $(LAMBDA).bsp $(BANSHEE)
	$(BANSHEE) $(LAMBDA).bsp $(LAMBDA)
	indent -npsl $@ 

$(LAMBDA).h: $(LAMBDA).c
	indent -npsl $@ 

TAGS: $(TAGFILES)
	etags $(TAGFILES)

# build banshee if necessary
$(BANSHEE): 
	$(MAKE) -C ../ banshee

clean:
	rm -f $(TARGETS) *~ *BAK	