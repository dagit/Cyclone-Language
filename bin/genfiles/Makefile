include ../../Makefile.inc

G_LIBS := $(addprefix g_, $(O_LIBS))

OLDCFLAGS := $(CFLAGS)
CFLAGS += -O3

CYC_LIB_PATH=$(CYCDIR)/bin/lib
CYC_BIN_PATH=$(CYCDIR)/bin

# general targets

install: $(ARCH) $(CYC_LIB_PATH)/$(CYCLIB)\
  $(addprefix $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/, nogc.a $(RUNTIME).$(O))\
  $(addprefix $(CYC_BIN_PATH)/, $(addsuffix $(EXE), cyclone cycdoc buildlib))
#	if [ -f "$(ARCH).patch" ]; then $(RM) -r $(ARCH); fi
$(CYC_LIB_PATH)/$(CYCLIB): $(ARCH)/lib/$(CYCLIB)
	cp -p $< $@
$(CYC_LIB_PATH)/nocheck_$(CYCLIB): $(ARCH)/lib/$(CYCLIB)
	cp -p $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/nogc.a: $(ARCH)/lib/nogc.a
	cp -p $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME).$(O): $(ARCH)/lib/$(RUNTIME).$(O)
	cp $< $@
$(CYC_BIN_PATH)/cyclone$(EXE): $(ARCH)/src/cyclone$(EXE)
	cp $< $@
$(CYC_BIN_PATH)/cycdoc$(EXE): $(ARCH)/src/cycdoc$(EXE)
	cp $< $@
$(CYC_BIN_PATH)/buildlib$(EXE): $(ARCH)/src/buildlib$(EXE)
	cp $< $@

install_a: $(ARCH) $(CYC_LIB_PATH)/libcyc_a.a \
  $(addprefix $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/, nogc_a.a $(RUNTIME)_a.$(O))

$(CYC_LIB_PATH)/libcyc_a.a: $(ARCH)/lib/libcyc_a.a
	cp -p $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/nogc_a.a: $(ARCH)/lib/nogc_a.a
	cp -p $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME)_a.$(O): $(ARCH)/lib/$(RUNTIME)_a.$(O)
	cp -p $< $@

install_pg: $(ARCH) $(CYC_LIB_PATH)/libcyc_pg.a \
  $(addprefix $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/, nogc_pg.a $(RUNTIME)_pg.$(O))

$(CYC_LIB_PATH)/libcyc_pg.a: $(ARCH)/lib/libcyc_pg.a
	cp $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/nogc_pg.a: $(ARCH)/lib/nogc_pg.a
	cp -p $< $@
$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME)_pg.$(O): $(ARCH)/lib/$(RUNTIME)_pg.$(O)
	cp -p $< $@

install_nocheck: $(ARCH) $(CYC_LIB_PATH)/libcyc_nocheck.a\
  $(addprefix $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/, nogc_nocheck.a $(RUNTIME).$(O))

$(CYC_LIB_PATH)/cyc-lib/$(ARCH)/nogc_nocheck.a: $(ARCH)/lib/nogc.a
	cp -p $< $@

$(CYC_LIB_PATH)/libcyc_nocheck.a: $(ARCH)/lib/libcyc_nocheck.a
	cp -p $< $@

# architecture-specific targets

define create_lib
-$(RM) $@
ar rc $@ $^
@echo Trying ranlib, if not found, probably ok to ignore error messages
-ranlib $@
endef

$(ARCH):
	./extract_patch $(PATCH_ARCH) $(ARCH)

$(ARCH)/lib/$(CYCLIB): $(addprefix $(ARCH)/lib/, $(O_LIBS) cstubs.$(O))
	$(create_lib)

$(ARCH)/lib/libcyc_a.a: $(addprefix $(ARCH)/lib/, $(A_LIBS) a_cstubs.$(O))
	$(create_lib)

$(ARCH)/lib/libcyc_nocheck.a: $(addprefix $(ARCH)/lib/, $(NOCHECK_LIBS) cstubs.$(O))
	$(create_lib)

$(ARCH)/lib/libcyc_pg.a: $(addprefix $(ARCH)/lib/, $(G_LIBS) a_cstubs.$(O))
	$(create_lib)

$(ARCH)/lib/nogc.a: $(ARCH)/lib/nogc.$(O)
	$(create_lib)

$(ARCH)/lib/nogc_a.a: $(ARCH)/lib/a_nogc.$(O)
	$(create_lib)

$(ARCH)/lib/nogc_pg.a: $(ARCH)/lib/g_nogc.$(O)
	$(create_lib)

$(ARCH)/lib/$(RUNTIME)_a.$(O): $(ARCH)/lib/a_$(RUNTIME).$(O)
	cp $< $@

$(ARCH)/lib/$(RUNTIME)_pg.$(O): $(ARCH)/lib/g_$(RUNTIME).$(O)
	cp $< $@

$(ARCH)/src/cyclone$(EXE): $(addprefix $(ARCH)/src/, $(O_SRCS) install_path.$(O)) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME).$(O) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/cycdoc$(EXE): $(addprefix $(ARCH)/src/, $(addsuffix .$(O), $(CYCDOC_SRCS))) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME).$(O) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/buildlib$(EXE): $(addprefix $(ARCH)/src/, $(addsuffix .$(O), $(BUILDLIB_SRCS)) install_path.$(O)) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/$(RUNTIME).$(O) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/cyc-lib/$(ARCH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/install_path.c: $(CYCDIR)/.version $(CYCDIR)/Makefile.inc
	 (echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Ccomp = \"$(CC)\";"; \
	  version=`cat $<`; \
	  echo "char *Cversion = \"$$version\";") > $@

clean_arch:
	$(RM) $(addprefix $(ARCH)/lib/, $(CYCLIB) libcyc_nocheck.a libcyc_a.a  $(O_LIBS) $(A_LIBS) $(NOCHECK_LIBS) nogc.[oa] a_nogc.$(O) nogc_a.a a_cstubs.$(O) cstubs.$(O) $(RUNTIME).$(O) a_$(RUNTIME).$(O) nocheck_$(RUNTIME).$(O))
	$(RM) $(addprefix $(ARCH)/src/, $(O_SRCS) cyclone$(EXE) cycdoc$(EXE) cycdoc.$(O) buildlib$(EXE) buildlib.$(O) install_path.*)

clean:
	for arch in $(ALL_ARCHS); do \
	if [ -d "$$arch" ]; then $(MAKE) clean_arch ARCH=$$arch; fi; \
	done

# pattern rules

$(ARCH)/src/%.$(O): $(ARCH)/src/%.c
	$(CC) $(OLDCFLAGS) -c -o $@ $<

a_%.$(O): %.c
	$(CC) -c -o $@ -DCYC_REGION_PROFILE $(CFLAGS) $<

g_%.$(O): %.c
	$(CC) -c -o $@ -pg $(CFLAGS) $<

nocheck_%.$(O): %.c
	$(CC) -c -o $@ -DNO_CYC_NULL_CHECKS -DNO_CYC_BOUNDS_CHECKS $(CFLAGS) $<

# for creating genfiles from patches

%.patch: %
	$(DIFF) -rbBcN --exclude=CVS $(PATCH_ARCH) $< > $@; exit 0
