include ../../Makefile.inc
BINDIR = $(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib
override CYCFLAGS += -I$(CYCDIR)/include $(LC_FLAG)
BUILDLIB=$(BINDIR)/buildlib -B$(CYC_LIB_PATH)/cyc-lib
CYCC=$(BINDIR)/$(CYCCOMP) -B$(CYC_LIB_PATH)/cyc-lib

ifeq ($LIBPCRE,"")
  LIBPCRE=$(shell $(CC) -print-file-name=libpcre.a)
endif

libpcre.a: libpcre.cys cycstubs.o cstubs.o
	cp $(LIBPCRE) $@
	$(AR) rs $@ cstubs.o cycstubs.o

pcre.h: libpcre.cys
	$(BUILDLIB) -d . $<

cycstubs.cyc: libpcre.cys
	$(BUILDLIB) -d . $<

cycstubs.o: cycstubs.cyc
	$(CYCC) -c cycstubs.cyc -I. $(CYCFLAGS)

cstubs.c: libpcre.cys
	$(BUILDLIB) -d . $<

clean:
	$(RM) cycstubs.cyc cstubs.c *.o pcre.h
	$(RM) *.i* BUILDLIB.LOG
	$(RM) libpcre.a

install: $(CYC_LIB_PATH)/libpcre.a $(CYC_LIB_PATH)/cyc-lib/$(build)/include/pcre.h

$(CYC_LIB_PATH)/libpcre.a: libpcre.a
	cp $< $@

$(CYC_LIB_PATH)/cyc-lib/$(build)/include/pcre.h: pcre.h
	cp $< $@

