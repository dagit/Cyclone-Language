
# Note: We're not doing anything with t.cyc t2.cyc t3.cyc t4.cyc 
#       testcases.cyc and toplev.cyc.  Should we be?

include ../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../bin/$(CYCCOMP)

CYCBISON=../bin/cycbison.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CFLAGS=-g -fwritable-strings -I.. 

CYC_FLAGS=-I ../lib -tc -toc -pp

LIB=../bin/$(OUT_PREFIX)$(CYCLIB)

all: $(OUT_PREFIX)cyctest $(OUT_PREFIX)foo $(OUT_PREFIX)hello \
     $(OUT_PREFIX)test_regions
	./$(OUT_PREFIX)test_regions
	./$(OUT_PREFIX)cyctest
	./$(OUT_PREFIX)foo < foo_input

$(OUT_PREFIX)hello: $(OUT_PREFIX)hello.o $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)merge_sort: $(OUT_PREFIX)merge_sort.o $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)test_regions: $(OUT_PREFIX)test_regions.o $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)cyctest: $(OUT_PREFIX)cyctest.o $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)foo: $(OUT_PREFIX)foo_tab.o $(OUT_PREFIX)foo.o $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)foo_tab.c : $(OUT_PREFIX)foo_tab.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<
$(OUT_PREFIX)foo_tab.cyc : foo.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@
$(OUT_PREFIX)foo_tab.h : foo.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $< 

clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), hello.o cyctest.o cyctest.c foo_tab.* foo.o foo.c *.exe)

clean:
	rm -f *hello.o *hello.c *bytes.o *bytes.c *cyctest.o *cyctest.c *foo_tab.* *foo.o *foo.c 
	rm -f *test_regions.o *test_regions.c
	rm -f *~ *.exe *.output *.stackdump
