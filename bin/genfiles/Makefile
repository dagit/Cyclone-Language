include ../../Makefile.inc

# I don't why these need to be repeated here, but they do
CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib
CYC_BIN_PATH=$(CYCDIR)/bin

#CFLAGS=-include $(CYC_LIB_PATH)/include/cyc_include.h

# general targets

install: $(ARCH) install_lib install_comp
	if [ -f "$(ARCH).patch" ]; then $(RM) -rf $(ARCH); fi
install_lib: $(ARCH)/lib/$(CYCLIB) $(ARCH)/lib/nogc.a
	cp $^ $(CYC_LIB_PATH)
install_a: $(ARCH)/lib/libcyc_a.a
	cp $^ $(CYC_LIB_PATH)
install_comp: $(ARCH)/src/cyclone$(EXE)
	cp $^ $(CYC_BIN_PATH)

# architecture-specific targets

$(ARCH):
	extract_patch $(PATH_ARCH) $(ARCH)
$(ARCH)/lib/$(CYCLIB): $(addprefix $(ARCH)/lib/, $(O_LIBS) $(RUNTIME).o)
	-rm -f $@
	ar rc $@ $^
	ranlib $@
$(ARCH)/lib/libcyc_a.a: $(addprefix $(ARCH)/lib/, $(A_LIBS) a_$(RUNTIME).o)
	-rm -f $@
	ar rc $@ $^
	ranlib $@
$(ARCH)/lib/nogc.a: $(ARCH)/lib/nogc.o
	-rm -f $@
	ar rc $@ $^
	ranlib $@
$(ARCH)/src/cyclone$(EXE): $(addprefix $(ARCH)/src/, $(O_SRCS) install_path.o) $(CYC_LIB_PATH)/$(CYCLIB) $(CYC_LIB_PATH)/gc.a
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARCH)/src/install_path.c: ../../Makefile.inc
	@(echo "char *Cdef_inc_path = \"$(INC_INSTALL)\";"; \
	  echo "char *Carch = \"$(ARCH)\";"; \
	  echo "char *Cdef_lib_path = \"$(LIB_INSTALL)\";"; \
	  echo "char *Ccflags = \"$(CFLAGS)\";"; \
	  echo "char *Ccomp = \"$(CC)\";") > $@

clean_arch:
	rm -f $(addprefix $(ARCH)/lib/, $(CYCLIB) libcyc_a.a nogc.a $(O_LIBS) $(A_LIBS) nogc.o $(RUNTIME).o a_$(RUNTIME).o)
	rm -f $(addprefix $(ARCH)/src/, $(O_SRCS) cyclone$(EXE) install_path.*)

clean:
	for arch in $(ALL_ARCHS); do \
	if [ -d "$$arch" ]; then $(MAKE) clean_arch ARCH=$$arch; fi; \
	done

# general rules

a_%.o: %.c
	$(CC) -c -o $@ -DCYC_REGION_PROFILE $(CFLAGS) $<

# for creating genfiles from patches

%.patch: %
	diff -rbBcN --exclude=CVS $(PATCH_ARCH) $< > $@; exit 0
