
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

all: $(addsuffix $(EXE), finger typerep_test genrep_test genrep_read genrep_write)

finger$(EXE): finger.o
	$(CYCC) $(CYCFLAGS) -o $@ $^ $(LDFLAGS) $(NET_LD_FLAGS)

unsafe_cast.o: unsafe_cast.c
	$(CC) -c $< -o $@

typerep_test$(EXE): typerep_test.o 
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

clean:
	$(RM) $(addsuffix .[oc], genrep_test genrep_read genrep_write finger)\
	$(RM) unsafe_cast.o
	$(RM) finger
	$(RM) genrep_write genrep_read genrep_test
	$(RM) typerep_test
	$(RM) *~ *.exe *.output *.stackdump
