
include ../Makefile.inc

# Override these variables, as well as CYCFLAGS, to test different
#   compilers/libraries (probably because you are bootstrapping the compiler) 
# (see targets test, test_bin, and test_src in parent directory's Makefile)
CYCBISON=cycbison$(EXE)
ifndef devel
CYCC=../bin/cyclone$(EXE)
else
CYCC=../build/boot/cyclone$(EXE)
CYCFLAGS=-I../lib -I../include -B../build/boot -pp -save-c -g --mergepaths 
endif
HAVE_PTHREAD=no
# test asm doesn't work for me on cygwin
EXES := $(addsuffix $(EXE), cyctest foo hello test_regions scheme-2 test_currgn histogram test_getopt zeroterm flatlist unique_patterns dynregion cinclude  multimethod  testasm testaqualsuccess scheme patterns refcnt_test rc md5 streambufftest testbget test_large_vc_terms)
 # testsexp structarray streambufftest ) 
#these are broken
#streambuff.cyc is broken badly ... it is unsound to pattern match on existentials with `RC inside them, wait until restrict
ifeq ($(HAVE_PTHREAD),yes)
EXES += $(addsuffix $(EXE),testexn)
endif

CYCTESTS=arg_suite.h				\
  array_suite.h					\
  attribute_suite.h				\
  bad_suite.h					\
  bitvec_suite.h				\
  buffer_suite.h				\
  ctype_suite.h					\
  dict_suite.h					\
  extern_c_suite.h				\
  filename_suite.h				\
  float_suite.h					\
  fn_suite.h					\
  hash_suite.h					\
  letv_suite.h					\
  list_suite.h					\
  pattern_match_suite.h				\
  pp_suite.h					\
  printf_suite.h				\
  queue_suite.h					\
  rope_suite.h					\
  scanf_suite.h					\
  set_suite.h					\
  signals_suite.h				\
  string_suite.h				\
  support.h					\
  swap_suite.h					\
  try_suite.h					\
  tuple_suite.h					\
  union_suite.h					\
  xarray_suite.h

all: $(EXES) 
	./test_regions
	./test_currgn
	./cyctest
	./foo < foo_input
	./histogram histogram.txt
	./zeroterm
#	./structarray
	./flatlist
	./md5 -q
	./test_getopt -abc123
	./streambufftest -q
	./unique_patterns
	./dynregion
	./scheme
	./scheme-2 --gc-messages
	./patterns
	./refcnt_test -q
	./cinclude
	./multimethod
#	./testsexp
	./rc
	./testasm
	./testbget
#	failtest 
ifeq ($(HAVE_PTHREAD),yes)
	./testexn
endif

# tests things that should be type errors
failtest:
	($(CYCC) $(CYCFLAGS) -c --manyerrors fail1.cyc 2> fail1.tmp) || true
	diff -w fail1.tmp fail1.out || true
	($(CYCC) $(CYCFLAGS) -c --manyerrors fail2.cyc 2> fail2.tmp) || true
	diff -w fail2.tmp fail2.out || true
	($(CYCC) $(CYCFLAGS) -c --manyerrors fail3.cyc 2> fail3.tmp) || true
	diff -w fail3.tmp fail3.out || true
	($(CYCC) $(CYCFLAGS) -c --manyerrors fail4.cyc 2> fail4.tmp) || true
	diff -w fail4.tmp fail4.out || true
	($(CYCC) $(CYCFLAGS) -c --manyerrors testaqualfail.cyc 2> taf.tmp) || true
	diff -w taf.tmp taf.out || true
	($(CYCC) $(CYCFLAGS) -c --manyerrors testaqualdefaults.cyc 2> tad.tmp) || true
	diff -w tad.tmp tad.out || true

cyctest$(EXE): cyctest.cyc $(addprefix cyctests/, $(CYCTESTS))
	$(CYCC) $(CYCFLAGS) -I. -Icyctests -o $@ $< $(LDFLAGS) -lm

foo$(EXE): foo_tab.o foo.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

foo_tab.o : foo_tab.cyc
	$(CYCC) $(CYCFLAGS) -c $<

foo_tab.cyc : foo.y
	$(CYCBISON) -v -d $< -o $@

foo_tab.h : foo.y
	$(CYCBISON) -v -d $< -o $@

pcredemo$(EXE): pcredemo.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) -lpcre

md5$(EXE): md5.o md5driver.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

streambufftest$(EXE): streambufftest.o streambuff.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

streambufftest.o: streambufftest.cyc
	$(CYCC) $(CYCFLAGS) -c $^

testbget$(EXE): ../lib/bget.c ../lib/bget.h testbget.c
	gcc -I../lib ../lib/bget.c testbget.c -lm -o testbget

scheme-2$(EXE): scheme-2.cyc
	$(CYCC) $(CYCFLAGS) $^ -nogc -lm -o $@ 

ifeq ($(HAVE_PTHREAD),yes)
testexn$(EXE): testexn.cyc
	$(CYCC) $(CYCFLAGS) $(LDFLAGS) -o $@ $^ -lpthread
endif

# This takes care of non-machine-generated versions (the .cyc is already there)
%.o : %.cyc
	$(CYCC) $(CYCFLAGS) -c $< 

#This takes care of any single-file program
%$(EXE): %.cyc
	$(CYCC) $(CYCFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(addsuffix .[oc], hello bytes cyctest foo histogram test_getopt typeref test_regions test_currgn unique_test zeroterm structarray flatlist md5 require streambufftest streambuff refcnt_test dynregion md5driver scheme scheme-2 unique_patterns cinclude testasm testexn multimethod testaqualsuccess test_large_vc_terms)
# testsexp)
	$(RM) foo_tab.* unsafe_cast.o pcredemo.o pcredemo.c pcredemo md5driver.o 
	$(RM) $(EXES) fail1.tmp fail2.tmp fail3.tmp fail4.tmp taf.tmp tad.tmp
	$(RM) *~ *.exe *.output *.stackdump
#	$(MAKE) clean -C benchmarks
#	$(MAKE) clean -C compress/cacm
#	$(MAKE) clean -C compress/ppm
	$(MAKE) clean -C shootout
#	$(MAKE) clean -C smlng
#	$(MAKE) clean -C tpop
