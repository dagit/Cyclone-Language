include ../../Makefile.inc

OLDCFLAGS := $(CFLAGS)
CFLAGS += -O3

# I don't know why these need to be repeated here, but they do
CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib
CYC_BIN_PATH=$(CYCDIR)/bin

# general targets

install: $(ARCH) $(addprefix $(CYC_LIB_PATH)/, $(CYCLIB) nogc.a)\
  $(CYC_BIN_PATH)/cyclone$(EXE)\
  $(CYC_BIN_PATH)/cycdoc$(EXE)\
  $(CYC_BIN_PATH)/buildlib$(EXE)
	if [ -f "$(ARCH).patch" ]; then $(RM) -r $(ARCH); fi
$(CYC_LIB_PATH)/$(CYCLIB): $(ARCH)/lib/$(CYCLIB)
	cp $< $@
$(CYC_LIB_PATH)/nocheck_$(CYCLIB): $(ARCH)/lib/$(CYCLIB)
	cp $< $@
$(CYC_LIB_PATH)/nogc.a: $(ARCH)/lib/nogc.a
	cp $< $@
$(CYC_BIN_PATH)/cyclone$(EXE): $(ARCH)/src/cyclone$(EXE)
	cp $< $@
$(CYC_BIN_PATH)/cycdoc$(EXE): $(ARCH)/src/cycdoc$(EXE)
	cp $< $@
$(CYC_BIN_PATH)/buildlib$(EXE): $(ARCH)/src/buildlib$(EXE)
	cp $< $@

install_a: $(ARCH) $(CYC_LIB_PATH)/libcyc_a.a $(CYC_LIB_PATH)/nogc_a.a

$(CYC_LIB_PATH)/libcyc_a.a: $(ARCH)/lib/libcyc_a.a
	cp $< $@
$(CYC_LIB_PATH)/nogc_a.a: $(ARCH)/lib/nogc_a.a
	cp $< $@


install_nocheck: $(ARCH) $(CYC_LIB_PATH)/libcyc_nocheck.a $(CYC_LIB_PATH)/nogc.a

$(CYC_LIB_PATH)/libcyc_nocheck.a: $(ARCH)/lib/libcyc_nocheck.a
	cp $< $@

# architecture-specific targets

$(ARCH):
	./extract_patch $(PATCH_ARCH) $(ARCH)

$(ARCH)/lib/$(CYCLIB): $(addprefix $(ARCH)/lib/, $(O_LIBS) $(RUNTIME).o)
	-$(RM) $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

$(ARCH)/lib/libcyc_a.a: $(addprefix $(ARCH)/lib/, $(A_LIBS) a_$(RUNTIME).o)
	-$(RM) $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

$(ARCH)/lib/libcyc_nocheck.a: $(addprefix $(ARCH)/lib/, $(NOCHECK_LIBS) $(RUNTIME).o)
	-$(RM) $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

$(ARCH)/lib/nogc.a: $(ARCH)/lib/nogc.o
	-$(RM) $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

$(ARCH)/lib/nogc_a.a: $(ARCH)/lib/a_nogc.o
	-$(RM) $@
	ar rc $@ $^
	@echo Trying ranlib, if not found, probably ok to ignore error messages
	-ranlib $@

$(ARCH)/src/cyclone$(EXE): $(addprefix $(ARCH)/src/, $(O_SRCS) install_path.$(O)) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/cycdoc$(EXE): $(addprefix $(ARCH)/src/, $(addsuffix .$(O), $(CYCDOC_SRCS))) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/buildlib$(EXE): $(addprefix $(ARCH)/src/, $(addsuffix .$(O), $(BUILDLIB_SRCS)) install_path.$(O)) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/install_path.c: ../../Makefile.inc
	@(echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Ccflags = \"$(OLDCFLAGS)\";"; \
	  echo "char *Ccomp = \"$(CC)\";") > $@

clean_arch:
	$(RM) $(addprefix $(ARCH)/lib/, $(CYCLIB) libcyc_nocheck.a libcyc_a.a  $(O_LIBS) $(A_LIBS) $(NOCHECK_LIBS) nogc.[oa] a_nogc.o nogc_a.a $(RUNTIME).o a_$(RUNTIME).o nocheck_$(RUNTIME).o)
	$(RM) $(addprefix $(ARCH)/src/, $(O_SRCS) cyclone$(EXE) cycdoc$(EXE) cycdoc.o buildlib$(EXE) buildlib.o install_path.*)

clean:
	for arch in $(ALL_ARCHS); do \
	if [ -d "$$arch" ]; then $(MAKE) clean_arch ARCH=$$arch; fi; \
	done

# specific file rules

$(ARCH)/src/%.o: $(ARCH)/src/%.c
	$(CC) $(OLDCFLAGS) -c -o $@ $<

# general rules

a_%.o: %.c
	$(CC) -c -o $@ -DCYC_REGION_PROFILE $(CFLAGS) $<

nocheck_%.o: %.c
	$(CC) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $(CFLAGS) $<

# for creating genfiles from patches

%.patch: %
	diff -rbBcN --exclude=CVS $(PATCH_ARCH) $< > $@; exit 0
