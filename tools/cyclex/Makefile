
# The only difference between initial building and bootstrapping is how
# we get lexer.cyc.  For the initial build we copy it from the genfiles
# subdirectory.  For bootstrapping, we create it from lexer.cyl.
# So a full "update to next version sequence" would look like:
# make all boot update.  But don't do the update until you're sure things
# look good!

include ../../Makefile.inc

PROG=cyclex.exe
BINDIR=../../bin

CYCC=../../bin/cyclone.exe
CYCBISON=../../bin/cycbison.exe

CFLAGS= -fwritable-strings -I../..
#CFLAGS=-O -fwritable-strings -I../..

CYC_FLAGS=-I ../../lib -tc -toc -pp

LIB=../../bin/cyclib.a

SRCS=syntax parser_tab lexer lexgen compact output main

C_SRCS=$(addsuffix .c, $(SRCS))
O_SRCS=$(addsuffix .o, $(SRCS))

install: all $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $(PROG) $(BINDIR)

all: $(PROG)

$(PROG): $(O_SRCS) $(LIB)
	gcc -g -o $@ $^ ../../bin/gc.a $(LDFLAGS)

boot:
	-rm -f lexer.cyc
	make $(PROG) BOOTSTRAP=X

diff:
	diff lexer.cyc genfiles/lexer.cyc

# careful -- this overwrites the cyclone file needed to bootstrap!
update: all genfiles/lexer.cyc


parser_tab.cyc: parser.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@
parser_tab.h: parser.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o parser_tab.cyc

ifdef BOOTSTRAP
lexer.cyc: lexer.cyl $(PROG)
	./$(PROG) $< $@

genfiles/lexer.cyc: lexer.cyc
	cp lexer.cyc genfiles/lexer.cyc
else
lexer.cyc: genfiles/lexer.cyc
	cp $< $@
endif

%.c: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $< 

clean:
	rm -f *.o *.c lexer.cyc parser_tab.cyc parser_tab.h *.output *.stackdump
	rm -f $(PROG)

lexer.cyc: parser_tab.h
parser_tab.c: parser.y
