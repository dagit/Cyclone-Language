
# The only difference between initial building and bootstrapping is
# how we get lexer.cyc.  For the initial build we copy it from the
# genfiles subdirectory.  For bootstrapping, we create it from
# lexer.cyl.  So a full "update to next version sequence" looks like:
# make all boot update.  But don't do update until things look good!

include ../../Makefile.inc

PROG=cyclex$(EXE)
BINDIR=$(CYCDIR)/bin

CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)

LC_FLAG =
# We put in the explicit -I and -B because install builds this tool before
# installing the include files and libraries.  Also we use a particular CYCC.
override CYCFLAGS += -I$(CYCDIR)/include -B$(BINDIR)/cyc-lib $(LC_FLAG)
CYCC:=$(BINDIR)/$(CYCCOMP) 

SRCS=syntax parser_tab lexer lexgen compact output main

O_SRCS=$(addsuffix .o, $(SRCS))

install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): $(O_SRCS) $(BINDIR)/cyc-lib/$(CYCLIB)
	$(CYCC) -B$(BINDIR)/cyc-lib $(LC_FLAG) -o $@ $(O_SRCS) $(LDFLAGS)

boot:
	-$(RM) lexer.cyc
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
	cp $< $@
else
lexer.cyc: genfiles/lexer.cyc
	cp $< $@
endif

%.o: %.cyc $(CYCC)
	$(CYCC) $(CYCFLAGS) -c $< 

clean:
	rm -f *.o *.c lexer.cyc parser_tab.cyc parser_tab.h *.output *.stackdump
	rm -f $(PROG) cyclex

lexer.cyc: parser_tab.h
parser_tab.c: parser.y
