
include ../Makefile.inc

# Override these variables, as well as CYCFLAGS, to test different
#   compilers/libraries (probably because you are bootstrapping the compiler) 
# (see targets test, test_bin, and test_src in parent directory's Makefile)
CYCBISON=cycbison$(EXE)
ifndef devel
CYCC=cyclone$(EXE)
else
CYCC=../build/boot/cyclone$(EXE)
CYCFLAGS=-I../lib -I../include -B../build/boot -pp -save-c
endif

all: $(addsuffix $(EXE), cyctest foo hello test_regions histogram test_getopt)
	./test_regions
	./cyctest
	./foo < foo_input
	./histogram histogram.txt
	./test_getopt -abc123

hello$(EXE): hello.o
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

test_getopt$(EXE): test_getopt.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

unsafe_cast.o: unsafe_cast.c
	$(CC) -c $< -o $@

typerep_test$(EXE): typerep_test.o 
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

equiv.o: equiv.c
	$(CC) -c $< -o $@

rep$(EXE):  rep.o equiv.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

genrep_write$(EXE):  genrep_write.o muttrep.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

genrep_read$(EXE):  genrep_read.o muttrep.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

genrep_test$(EXE):  genrep_test.o muttrep.o 
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) -lxml

# This takes care of non-machine-generated versions (the .cyc is already there)
%.o : %.cyc
	$(CYCC) $(CYCFLAGS) -c $< 

#This takes care of any single-file program
%$(EXE): %.cyc
	$(CYCC) $(CYCFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(addsuffix .[oc], genrep_test genrep_read genrep_write hello bytes cyctest foo histogram test_getopt typeref typerep_test test_regions)\
	$(RM) foo_tab.* unsafe_cast.o
	$(RM) hello cyctest foo histogram merge_sort test_getopt
	$(RM) test_regions genrep_write genrep_read genrep_test rep
	$(RM) typerep_test
	$(RM) *~ *.exe *.output *.stackdump
#	$(MAKE) clean -C benchmarks
#	$(MAKE) clean -C compress/cacm
#	$(MAKE) clean -C compress/ppm
	$(MAKE) clean -C shootout
	$(MAKE) clean -C smlng
#	$(MAKE) clean -C tpop
#	$(MAKE) clean -C misc
