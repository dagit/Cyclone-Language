
include ../../../Makefile.inc

CFLAGS=-include ../../cyc-lib/include/cyc_include.h

all: $(CYCLIB) nogc.a

$(CYCLIB): $(O_LIBS) $(RUNTIME).o
	-rm -f $@
	ar rcs $@ $^

nogc.a: nogc.o
	ar rcs $@ $^

clean:
	rm -f $(O_LIBS) $(RUNTIME).o $(CYCLIB) nogc.o nogc.a *~
	rm -f $(A_LIBS) a_$(RUNTIME).o libcyc_a.a a_nogc.o a_nogc.a *~

# For allocation profiling
all_a: libcyc_a.a nogc_a.a

A_LIBS=$(addprefix a_, $(O_LIBS))

libcyc_a.a: $(A_LIBS) a_$(RUNTIME).o
	-rm -f $@
	ar rcs $@ $^

nogc_a.a: a_nogc.o
	ar rcs $@ $^

a_%.o: %.c
	$(CC) -c -o $@ -DCYC_REGION_PROFILE $(CFLAGS) $<
