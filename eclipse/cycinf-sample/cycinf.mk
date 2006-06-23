#######################################
# Cyclone with Inference `make' library
# By Matt McCutchen

.DELETE_ON_ERROR:

# Individual Cyclone projects may override these defaults

CYCINF=cyclone-inf
INFFLAGS=--inf --nomerge --ovfat -detailedlocation --nolower
RESFLAGS=--resolve
CFLAGS=

# Stages of compilation:
# foo.cyc (and included files) -> preprocess and introduce inference -> .cych/foo.cyc
# Introduction of cvars in all sources leads to .cych/cvars
# .cych/foo.cyc -> resolve -> .cych/foo.c, foo.o

# SOURCES=foo.cyc bar.cyc
INFERREDS=$(patsubst %.cyc,.cych/%.cyc,$(SOURCES))
CYCFRONTEDS=$(patsubst %.cyc,.cych/%.c,$(SOURCES))
OBJS=$(patsubst %.cyc,%.o,$(SOURCES))

# Rule that requires reintroducing cvars when any source file changes.
.cych/start-stamp: $(SOURCES)
	mkdir -p .cych
	rm -f .cych/cvars
	touch $@

# Rule that introduces cvars.
# If compilation fails, the cvars file may be destroyed, so we kill the start stamp.
# That causes us to remove the cvars file and start over.
.cych/%.cyc: %.cyc .cych/start-stamp
	$(CYCINF) $(INFFLAGS) $(CFLAGS) $< || (a=$$?; rm -f .cych/start-stamp; exit $$a)

# Dependencies for the cvars file; if it is up-to-date, introduction of inference is done
.cych/cvars-done: .cych/start-stamp $(patsubst %.cyc,.cych/%.cyc,$(SOURCES))
	touch .cych/cvars-done

# Rule that resolves cvars.
%.o: .cych/%.cyc .cych/cvars-done
	$(CYCINF) $(RESFLAGS) $(CFLAGS) -c $<

# Use $(OBJS) in your final linking command.

clean1:
	rm -rf $(OBJS) $(CYCFRONTEDS) $(INFERREDS) .cych
clean: clean1
