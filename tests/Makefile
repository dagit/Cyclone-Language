
# Note: this tests the compiler and library in ../src and ../lib,
# not the compiler in ../bin

# Note: We're not doing anything with t.cyc t2.cyc t3.cyc t4.cyc 
#       testcases.cyc and toplev.cyc.  Should we be?

include ../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../bin/$(CYCCOMP)

CYCBISON=../bin/cycbison$(EXE)

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CYC_FLAGS=-I../lib -B../lib -g

LIB=../bin/cyc-lib/$(OUT_PREFIX)$(CYCLIB)

all: $(CYCC) $(LIB) \
  $(OUT_PREFIX)cyctest$(EXE) \
  $(OUT_PREFIX)foo$(EXE) \
  $(OUT_PREFIX)hello$(EXE) \
  $(OUT_PREFIX)test_regions$(EXE) \
  $(OUT_PREFIX)histogram$(EXE)
	./$(OUT_PREFIX)test_regions
	./$(OUT_PREFIX)cyctest
	./$(OUT_PREFIX)foo < foo_input
	./$(OUT_PREFIX)histogram histogram.txt

$(OUT_PREFIX)hello$(EXE): $(OUT_PREFIX)hello.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)
#	gcc -g -o $@ $^ ../bin/gc.a $(LDFLAGS)

$(OUT_PREFIX)merge_sort$(EXE): $(OUT_PREFIX)merge_sort.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

$(OUT_PREFIX)test_regions$(EXE): $(OUT_PREFIX)test_regions.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

$(OUT_PREFIX)cyctest$(EXE): $(OUT_PREFIX)cyctest.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

$(OUT_PREFIX)foo$(EXE): $(OUT_PREFIX)foo_tab.o $(OUT_PREFIX)foo.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

$(OUT_PREFIX)foo_tab.o : $(OUT_PREFIX)foo_tab.cyc
	$(CYCC) $(CYC_FLAGS) -c $<

$(OUT_PREFIX)foo_tab.cyc : foo.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@

$(OUT_PREFIX)foo_tab.h : foo.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@

$(OUT_PREFIX)histogram$(EXE): $(OUT_PREFIX)histogram.o
	$(CYCC) $(CYC_FLAGS) -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o : %.cyc
	$(CYCC) $(CYC_FLAGS) -c $< 

clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), hello hello.o cyctest cyctest.o cyctest.c foo foo_tab.* foo.o foo.c histogram histogram.o histogram.c merge_sort merge_sort.o *.exe)

clean:
	rm -f *hello.o *hello.c *bytes.o *bytes.c *cyctest.o *cyctest.c *foo_tab.* *foo.o *foo.c *histogram.o *histogram.c
	rm -f *test_regions.o *test_regions.c
	rm -f hello cyctest foo histogram merge_sort
	rm -f *~ *.exe *.output *.stackdump
