# We modified the bison source-code from the Free Software Foundation 
# (copyright below) so that it is written in Cyclone and generates Cyclone code.

# Makefile for bison
# Copyright (C) 1988, 1989, 1991, 1993 Bob Corbett and Free Software Foundation,
# Inc.
# 
# This file is part of Bison, the GNU Compiler Compiler.
# 
# Bison is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# Bison is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Bison; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

include ../../Makefile.inc
PROG = cycbison$(EXE)
BINDIR = $(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib

# hack so executable can find cycbison.simple
BISONDIR=$(shell pwd)
# names of parser files
PFILE=cycbison.simple
# DO NOT USE bison.hairy! NOT PORTED! (maybe someday)
PFILE1=bison.hairy
PFILES = -DXPFILE=\"$(BISONDIR)/$(PFILE)\" -DXPFILE1=\"$(BISONDIR)/$(PFILE1)\"

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool
# before installing the include files and libraries.  Also we use a
# particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

SRCS = main getargs version warshall files gram \
symtab closure LR0 reader print conflicts lex lalr reduce \
nullable output derives

O_SRCS = $(addsuffix .o, $(SRCS))

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

# The -B tells the compiler to use the right gc.a and runtime_cyc.o
# The -L tells the compiler to use the right libcyc.a
$(PROG): $(O_SRCS) $(CYC_LIB_PATH)/$(CYCLIB)
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib $(LC_FLAG) -o $@ $(O_SRCS) $(LDFLAGS)

# This file is different to pass the parser file names to the compiler.
files.o: files.cyc
	$(CYCC) $(CYCFLAGS) $(PFILES) -c files.cyc
%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $<

clean:
	$(RM) *.o *.c core cycbison *~ *.stackdump $(PROG)

LR0.o: system.h machine.h new.h gram.h state.h
closure.o: system.h machine.h new.h gram.h
conflicts.o: system.h machine.h new.h files.h gram.h state.h
derives.o: system.h new.h types.h gram.h
files.o: system.h files.h new.h gram.h
getargs.o: system.h files.h
lalr.o: system.h machine.h types.h state.h new.h gram.h
lex.o: system.h files.h symtab.h lex.h
main.o: system.h machine.h
nullable.o: system.h types.h gram.h new.h
output.o: system.h machine.h new.h files.h gram.h state.h
print.o: system.h machine.h new.h files.h gram.h state.h
reader.o: system.h files.h new.h symtab.h lex.h gram.h
reduce.o: system.h machine.h files.h new.h gram.h
symtab.o: system.h new.h symtab.h gram.h
warshall.o: system.h machine.h
