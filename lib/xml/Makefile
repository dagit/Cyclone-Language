CYCDIR=../..

include $(CYCDIR)/Makefile.inc

BINDIR=$(CYCDIR)/bin

CYCC=$(CYCDIR)/bin/$(CYCCOMP)
CYCBISON=$(CYCDIR)/bin/cycbison$(EXE)
CYCLEX=$(CYCDIR)/bin/cyclex$(EXE)

CYC_FLAGS=-I.. -B..

SRCS=xml xmlparse_tab xmlscan xmldump

OBJS=$(addsuffix .o, $(SRCS))
CSRCS=$(addsuffix .c, $(SRCS))

all: xmlecho$(EXE) libxml.a

xmlecho$(EXE): $(OBJS) xmlecho.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

test: xmlecho$(EXE)
	./xmlecho test.xml

libxml.a: $(OBJS)
	-rm -f $@
	ar qcs $@ $^

csrc: $(CSRCS)

clean:
	rm -f *.o *.c *.output *.stackdump
	rm -f xmlscan.cyc
	rm -f xmlparse_tab.cyc xmlparse_tab.h
	rm -f libxml.a
	rm -f xmlecho xmlecho.exe

%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

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
