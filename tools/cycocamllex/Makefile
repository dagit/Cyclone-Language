
# We took the ocamllex source-code from INRIA (copyright below)
# and hacked the source code to produce Cyclone code instead of OCaml code.
# The resulting program is a lexer-generator for Cyclone.
# To rebuild the tool, OCaml 3.0 (including ocamlopt, ocamllex and ocamlyacc)
# needs to be installed.
# Nothing should be necessary to run the resulting executable.

#########################################################################
#                                                                       #
#                            Objective Caml                             #
#                                                                       #
#            Xavier Leroy, projet Cristal, INRIA Rocquencourt           #
#                                                                       #
#   Copyright 1999 Institut National de Recherche en Informatique et    #
#   en Automatique.  All rights reserved.  This file is distributed     #
#   under the terms of the Q Public License version 1.0.                #
#                                                                       #
#########################################################################

# The lexer generator
PROG=cycocamllex.exe
BINDIR=../../bin

CAMLC=ocamlopt
COMPFLAGS=
LINKFLAGS=
CAMLYACC=ocamlyacc
YACCFLAGS=
CAMLLEX=ocamllex
CAMLDEP=ocamldep
DEPFLAGS=

OBJS=parser.cmx lexer.cmx lexgen.cmx compact.cmx output.cmx main.cmx

all: $(PROG)

$(PROG): $(OBJS)
	$(CAMLC) $(LINKFLAGS) -o $@ $^

install: all
	cp $(PROG) $(BINDIR)

clean::
	rm -f cycocamllex.exe
	rm -f *.cmx *.cmi *.obj *~

parser.ml parser.mli: parser.mly
	$(CAMLYACC) $(YACCFLAGS) parser.mly

clean::
	rm -f parser.ml parser.mli

beforedepend:: parser.ml parser.mli

lexer.ml: lexer.mll
	$(CAMLLEX) lexer.mll

clean::
	rm -f lexer.ml

beforedepend:: lexer.ml

.SUFFIXES:
.SUFFIXES: .ml .cmx .mli .cmi

.ml.cmx:
	$(CAMLC) -c $(COMPFLAGS) $<

.mli.cmi:
	$(CAMLC) -c $(COMPFLAGS) $<

depend: beforedepend
	$(CAMLDEP) *.mli *.ml > .depend

include .depend
