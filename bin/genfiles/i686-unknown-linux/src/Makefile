include ../../../../Makefile.inc

CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib

CFLAGS=-include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-fwritable-strings -include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-O -include $(CYC_INCLUDE_PATH)/include/cyc_include.h

PROG=cyclone$(EXE)

all: $(PROG)

$(PROG): $(O_SRCS) install_path.o $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a
	$(MAKE) -C ../lib
	$(CC) -O -o $@ $^ $(LDFLAGS)
	$(RM) install_path.c install_path.o

install_path.c:
	@(echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";") > $@

clean:
	rm -f $(O_SRCS) $(PROG) *~ install_path.*
