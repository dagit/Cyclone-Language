
# The only difference between initial building and bootstrapping is how
# we get lexer.cyc.  For the initial build we copy it from the genfiles
# subdirectory.  For bootstrapping, we create it from lexer.cyl.
# So a full "update to next version sequence" would look like:
# make all boot update.  But don't do the update until you're sure things
# look good!

CYCDIR = ../..

include $(CYCDIR)/Makefile.inc

PROG=cyclex$(EXE)
BINDIR=$(CYCDIR)/bin

CYCC=$(CYCDIR)/bin/cyclone$(EXE)
CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)

LC_FLAG =
CYC_FLAGS += -I$(CYCDIR)/include -B$(BINDIR)/cyc-lib $(LC_FLAG)

SRCS=syntax parser_tab lexer lexgen compact output main

O_SRCS=$(addsuffix .o, $(SRCS))

install: all $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $(PROG) $(BINDIR)

all: $(PROG)

$(PROG): $(O_SRCS)
	$(CYCC) -B$(BINDIR)/cyc-lib $(LC_FLAG) -g -o $@ $^ $(LDFLAGS)

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

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $< 

clean:
	rm -f *.o *.c lexer.cyc parser_tab.cyc parser_tab.h *.output *.stackdump
	rm -f $(PROG) cyclex

lexer.cyc: parser_tab.h
parser_tab.c: parser.y
