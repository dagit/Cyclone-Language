
# We took the bison source-code from the Free Software Foundation 
# (copyright below) and hacked the source code to produce Cyclone code instead
# of C code.
# The resulting program is a parser-generator for Cyclone.
# To rebuild the tool, gcc needs to be installed.

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

PROG=cycbison.exe
BINDIR=../../bin

# hack so executable can find cycbison.simple
BISONDIR=$(shell pwd)

CC = gcc 
CFLAGS = -g
LDFLAGS =

# names of parser files
PFILE=cycbison.simple
  # DO NOT USE bison.hairy! NOT PORTED! (maybe someday)
PFILE1=bison.hairy

PFILES = -DXPFILE=\"$(BISONDIR)/$(PFILE)\" -DXPFILE1=\"$(BISONDIR)/$(PFILE1)\"

OBJECTS = LR0.o allocate.o closure.o conflicts.o derives.o files.o	\
	  getargs.o gram.o lalr.o lex.o					\
	  main.o nullable.o output.o print.o reader.o reduce.o symtab.o	\
	  warshall.o version.o						\
	  getopt.o getopt1.o alloca.o

all: $(PROG)

install: all
	cp $(PROG) $(BINDIR)

$(PROG): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o core cycbison *~ cycbison.exe

# This file is different to pass the parser file names to the compiler.
files.o: files.c
	$(CC) -c $(PFILES) $(CPPFLAGS) $(CFLAGS) files.c

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
