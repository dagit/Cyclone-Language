include ../../Makefile.inc

ifndef ARCH
$(error "Must have ARCH variable defined to properly compile")
else
ifndef TARGET
TARGET := $(ARCH)
endif
endif

# use the installed compiler ..
CYC_BIN_PATH=$(CYCDIR)/bin

# ... but the source libraries
CYC_LIB_PATH=$(CYCDIR)/lib
CYC_INC_PATH=$(CYCDIR)/lib

CYCC=$(CYC_BIN_PATH)/$(CYCCOMP)
CYCBISON=$(CYC_BIN_PATH)/cycbison$(EXE)
CYCLEX=$(CYC_BIN_PATH)/cyclex$(EXE)

# for cross-compiling
CPP_FLAG := -use-cpp '$(CYCDIR)/bin/cyccpp $(CYCDIR)/config/arch/$(ARCH) $(CYCDIR)/config/arch/$(TARGET)'

CYC_FLAGS=-I$(CYC_INC_PATH) -B$(CYC_LIB_PATH) $(CPP_FLAG)

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
	ar rcs $@ $^

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
