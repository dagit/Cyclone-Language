include ../../../../Makefile.inc

CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib

CFLAGS=-include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-fwritable-strings -include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-g -fwritable-strings -include $(CYC_LIB_PATH)/include/cyc_include.h
#CFLAGS=-O -include $(CYC_INCLUDE_PATH)/include/cyc_include.h

PROG=cyclone$(EXE)

all: $(PROG)

$(PROG): $(O_SRCS)
	$(MAKE) -C ../lib
	$(CC) -O -o $@ $^ $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a $(LDFLAGS)

clean::
	rm -f $(O_SRCS) $(PROG) *~ 
