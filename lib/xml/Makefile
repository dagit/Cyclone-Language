include ../../Makefile.inc

# use the installed compiler ..
CYC_BIN_PATH=$(CYCDIR)/bin
CYC_LIB_PATH=$(CYCDIR)/bin/lib
CYC_INC_PATH=$(CYCDIR)/include

CYCC=$(CYC_BIN_PATH)/$(CYCCOMP)
CYCBISON=$(CYC_BIN_PATH)/cycbison$(EXE)
CYCLEX=$(CYC_BIN_PATH)/cyclex$(EXE)

override CYC_FLAGS+= -I$(CYC_INC_PATH) -B$(CYC_LIB_PATH)/cyc-lib

SRCS=xml xmlparse_tab xmlscan xmldump xmlmarshal

OBJS=$(addsuffix .o, $(SRCS))
CSRCS=$(addsuffix .c, $(SRCS))

all: xmlecho$(EXE) libxml.a

install: all $(CYCDIR)/bin/cyc-lib/libxml.a

$(CYCDIR)/bin/cyc-lib/libxml.a: libxml.a
	cp -p libxml.a $(CYC_LIB_PATH)

xmlecho$(EXE): $(OBJS) xmlecho.o
	$(CYCC) -L$(CYC_LIB_PATH) -B$(CYC_LIB_PATH)/cyc-lib -o $@ $^ $(LDFLAGS)

test: xmlecho$(EXE)
	./xmlecho test.xml

libxml.a: $(OBJS)
	-rm -f $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

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
	$(CYCBISON) -d $< -o $@
xmlparse_tab.h: xmlparse.y
	$(CYCBISON) -d $< -o xmlparse_tab.cyc

xmlscan.cyc: xmlparse_tab.h
xmlscan.cyc: xmlscan.cyl
	$(CYCLEX) $< $@

xml.c: xml.h
xmlparse_tab.c: xmlparse.h xml.h xmlscan.h
xmldump.c: xmldump.h xml.h 
xmlscan.c: xmlscan.h xml.h xmlparse.h xmlparse_tab.h
xmlecho.c: xmldump.h xml.h xmlscan.h xmlparse.h
xmlmarshal.c: xmlmarshal.h xml.h
