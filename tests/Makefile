
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

EXES=$(addsuffix $(EXE), cyctest foo hello test_regions histogram test_getopt zeroterm structarray flatlist md5 unique_patterns dynregion refcnt_test require streambufftest scheme)

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

all: $(EXES) failtest
	./test_regions
	./cyctest
	./foo < foo_input
	./histogram histogram.txt
	./zeroterm
	./structarray
	./flatlist
	./md5 -q
	./require
	./test_getopt -abc123
	./streambufftest -q
	./unique_patterns
	./dynregion
	./scheme
	./refcnt_test -q

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

hello$(EXE): hello.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

test_regions$(EXE): test_regions.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS)

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
	$(RM) $(addsuffix .[oc], hello bytes cyctest foo histogram test_getopt typeref test_regions unique_test zeroterm structarray flatlist md5 require streambufftest streambuff refcnt_test dynregion md5driver scheme unique_patterns)
	$(RM) foo_tab.* unsafe_cast.o pcredemo.o pcredemo.c pcredemo md5driver.o
	$(RM) $(EXES) fail1.tmp fail2.tmp fail3.tmp fail4.tmp
	$(RM) *~ *.exe *.output *.stackdump
#	$(MAKE) clean -C benchmarks
#	$(MAKE) clean -C compress/cacm
#	$(MAKE) clean -C compress/ppm
	$(MAKE) clean -C shootout
#	$(MAKE) clean -C smlng
#	$(MAKE) clean -C tpop
