
include ../Makefile.inc

# Override these variables to test different compilers/libraries (bootstrapping)
# (see targets test, test_bin, and test_src in parent directory's Makefile)
CYCC=cyclone$(EXE)
CYCBISON=cycbison$(EXE)
CYCFLAGS=

all: $(addsuffix $(EXE), cyctest foo hello test_regions histogram finger test_getopt)
	./test_regions
	./cyctest
	./foo < foo_input
	./histogram histogram.txt
	./test_getopt -abc123

hello$(EXE): hello.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

merge_sort$(EXE): merge_sort.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

test_regions$(EXE): test_regions.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

cyctest$(EXE): cyctest.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) -lm

foo$(EXE): foo_tab.o foo.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

foo_tab.o : foo_tab.cyc
	$(CYCC) $(CYCFLAGS) -c $<

foo_tab.cyc : foo.y
	$(CYCBISON) -v -d $< -o $@

foo_tab.h : foo.y
	$(CYCBISON) -v -d $< -o $@

histogram$(EXE): histogram.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

finger$(EXE): finger.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) $(NET_LD_FLAGS)

test_getopt$(EXE): test_getopt.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

unsafe_cast.o: unsafe_cast.c
	$(CC) -c $< -o $@

typerep_test$(EXE): typerep_test.o 
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

genrep_test$(EXE):  genrep_test.o muttrep.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
%.o : %.cyc
	$(CYCC) $(CYCFLAGS) -c $< 

clean_prefix:
	$(RM) hello hello.o cyctest cyctest.o cyctest.c foo foo_tab.* foo.o foo.c histogram histogram.o histogram.c merge_sort merge_sort.o test_getopt.o test_getopt *.exe

clean:
	$(RM) *hello.o *hello.c *bytes.o *bytes.c *cyctest.o *cyctest.c *foo_tab.* *foo.o *foo.c *histogram.o *histogram.c *finger.o *finger.c *test_getopt.c *test_getopt.o *typerep.[oc] *typerep_test.[oc] unsafe_cast.o
	$(RM) *test_regions.o *test_regions.c
	$(RM) hello cyctest foo histogram merge_sort finger test_getopt
	$(RM) *~ *.exe *.output *.stackdump
#	$(MAKE) clean -C benchmarks
#	$(MAKE) clean -C compress/cacm
#	$(MAKE) clean -C compress/ppm
	$(MAKE) clean -C shootout
	$(MAKE) clean -C smlng
#	$(MAKE) clean -C tpop
