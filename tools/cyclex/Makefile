
# The only difference between initial building and bootstrapping is how
# we get lexer.cyc.  For the initial build we copy it from the genfiles
# subdirectory.  For bootstrapping, we create it from lexer.cyl.
# So a full "update to next version sequence" would look like:
# make all boot update.  But don't do the update until you're sure things
# look good!

PROG=cyclex.exe
BINDIR=../../bin

CYCC=../../bin/cyclone.exe
CYCBISON=../../bin/cycbison.exe

CFLAGS=-g -fwritable-strings -I../..

CYC_FLAGS=-I ../../lib -tc -toc -pp

LIB=../../bin/cyclib.a

SRCS=syntax parser_tab lexer lexgen compact output main

C_SRCS=$(addsuffix .c, $(SRCS))
O_SRCS=$(addsuffix .o, $(SRCS))

install: all
	cp $(PROG) $(BINDIR)

all: $(PROG)

$(PROG): $(O_SRCS) $(LIB)
	gcc -g -o $@ $^ ../../bin/gc.a

boot:
	-rm -f lexer.cyc
	make $(PROG) BOOTSTRAP=X

diff:
	diff lexer.cyc genfiles/lexer.cyc

# careful -- this overwrites the cyclone file needed to bootstrap!
update: all
	cp lexer.cyc genfiles/lexer.cyc

parser_tab.cyc: parser.y
	$(CYCBISON) -v -d $< -o $@
parser_tab.h: parser.y
	$(CYCBISON) -v -d $< -o parser_tab.cyc

ifdef BOOTSTRAP
lexer.cyc: lexer.cyl
	./$(PROG) $< $@
else
lexer.cyc: genfiles/lexer.cyc
	cp $< $@
endif

%.c: %.cyc
	$(CYCC) $(CYC_FLAGS) $< > $@

clean:
	rm -f *.o *.c lexer.cyc parser_tab.cyc parser_tab.h *.output *.stackdump
	rm -f $(PROG)

lexer.cyc: parser_tab.h
parser_tab.c: parser.y