
include ../Makefile.inc

# Override these variables, as well as CYCFLAGS, to test different
#   compilers/libraries (probably because you are bootstrapping the compiler) 
# (see targets test, test_bin, and test_src in parent directory's Makefile)
CYCBISON=cycbison$(EXE)
ifndef devel
CYCC=cyclone$(EXE)
else
CYCC=../build/boot/cyclone$(EXE)
CYCFLAGS=-I../lib -I../include -B../build/boot -pp -save-c -g
endif

EXES=$(addsuffix $(EXE), cyctest foo hello test_regions histogram test_getopt zeroterm structarray flatlist require md5 streambufftest)

all: $(EXES) failtest
	./test_regions
	./cyctest
	./foo < foo_input
	./histogram histogram.txt
	./zeroterm
	./structarray
	./flatlist
	./md5 abcdefg
	./require
	./test_getopt -abc123
	./streambufftest

# tests things that should be type errors
failtest:
	($(CYCC) -c --manyerrors fail1.cyc 2> fail1.tmp) || true
	diff fail1.tmp fail1.out || true

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

pcredemo$(EXE): pcredemo.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) -lpcre

zeroterm$(EXE): zeroterm.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

structarray$(EXE): structarray.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

flatlist$(EXE): flatlist.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

md5$(EXE): md5.o md5driver.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

require$(EXE): require.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

streambufftest$(EXE): streambufftest.o streambuff.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

# This takes care of non-machine-generated versions (the .cyc is already there)
%.o : %.cyc
	$(CYCC) $(CYCFLAGS) -c $< 

#This takes care of any single-file program
%$(EXE): %.cyc
	$(CYCC) $(CYCFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(addsuffix .[oc], hello bytes cyctest foo histogram test_getopt typeref test_regions unique_test zeroterm structarray flatlist md5 require streambufftest streambuff)
	$(RM) foo_tab.* unsafe_cast.o pcredemo.o pcredemo.c pcredemo
	$(RM) $(EXES) fail1.tmp
	$(RM) *~ *.exe *.output *.stackdump
#	$(MAKE) clean -C benchmarks
#	$(MAKE) clean -C compress/cacm
#	$(MAKE) clean -C compress/ppm
	$(MAKE) clean -C shootout
#	$(MAKE) clean -C smlng
#	$(MAKE) clean -C tpop
