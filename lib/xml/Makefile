CYCDIR=../..

include $(CYCDIR)/Makefile.inc

BINDIR=$(CYCDIR)/bin

CYCC=$(CYCDIR)/bin/cyclone.exe
CYCBISON=$(CYCDIR)/bin/cycbison.exe
CYCLEX=$(CYCDIR)/bin/cyclex.exe

CFLAGS= -fwritable-strings -I$(CYCDIR)

CYC_FLAGS=-I $(CYCDIR)/lib -tc -toc -pp

LIB=../cyclib.a

SRCS=xml xmlparse_tab xmlscan xmldump

OBJS=$(addsuffix .o, $(SRCS))
CSRCS=$(addsuffix .c, $(SRCS))

all: xmlecho.exe libxml.a

xmlecho.exe: $(OBJS) xmlecho.o $(LIB)
	gcc -o $@ $^ $(CYCDIR)/bin/gc.a $(LDFLAGS)

test: xmlecho.exe
	./xmlecho test.xml

libxml.a: $(OBJS)
	-rm -f $@
	ar qc $@ $^
	ranlib $@

csrc: $(CSRCS)

clean:
	rm -f *.o *.c *.output *.stackdump
	rm -f xmlscan.cyc
	rm -f xmlparse_tab.cyc xmlparse_tab.h
	rm -f libxml.a
	rm -f xmlecho.exe

%.c : %.cyc
	$(CYCC) $(CYC_FLAGS) -o $@ $< 

xmlparse_tab.cyc: xmlparse.y
	$(CYCBISON) -v -d $< -o $@
xmlparse_tab.h: xmlparse.y
	$(CYCBISON) -v -d $< -o xmlparse_tab.cyc

xmlscan.cyc: xmlparse_tab.h
xmlscan.cyc: xmlscan.cyl
	$(CYCLEX) $< $@

xml.c: xml.h
xmlparse_tab.c: xmlparse.h xml.h xmlscan.h
xmldump.c: xmldump.h xml.h 
xmlscan.c: xmlscan.h xml.h xmlparse.h xmlparse_tab.h
xmlecho.c: xmldump.h xml.h xmlscan.h xmlparse.h
