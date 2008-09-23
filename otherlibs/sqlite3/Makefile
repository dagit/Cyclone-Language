include ../../Makefile.inc
BINDIR = $(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib
override CYCFLAGS += -I$(CYCDIR)/include $(LC_FLAG)
BUILDLIB=$(BINDIR)/buildlib -B$(CYC_LIB_PATH)/cyc-lib
CYCC=$(BINDIR)/$(CYCCOMP) -B$(CYC_LIB_PATH)/cyc-lib

ifeq ($LIBSQLITE3,"")
  LIBSQLITE3=$(shell $(CC) -print-file-name=libsqlite3.a)
endif

libsqlite3.a: libsqlite3.cys cycstubs.o cstubs.o
	cp $(LIBSQLITE3) $@
	$(AR) rs $@ cstubs.o cycstubs.o

sqlite3test: sqlite3test.cyc libsqlite3.a
	$(CYCC) -o $@ $^ -lpthread

sqlite3.h: libsqlite3.cys
	$(BUILDLIB) -d . $<

cycstubs.cyc: libsqlite3.cys
	$(BUILDLIB) -d . $<

cycstubs.o: cycstubs.cyc sqlite3.h
	$(CYCC) -c cycstubs.cyc -I. $(CYCFLAGS)

cstubs.c: libsqlite3.cys
	$(BUILDLIB) -d . $<

clean:
	$(RM) cycstubs.cyc cstubs.c *.o sqlite3.h
	$(RM) *.i* BUILDLIB.LOG
	$(RM) libsqlite3.a
	$(RM) *.db
	$(RM) sqlite3test

install: $(CYC_LIB_PATH)/libsqlite3.a $(CYC_LIB_PATH)/cyc-lib/$(build)/include/sqlite3.h

.PHONY: clean install fixlib

# To fix the library for use on Mac OS X:
fixlib:
	ranlib $(CYC_LIB_PATH)/libsqlite3.a

$(CYC_LIB_PATH)/libsqlite3.a: libsqlite3.a
	cp $< $@

$(CYC_LIB_PATH)/cyc-lib/$(build)/include/sqlite3.h: sqlite3.h
	cp $< $@
