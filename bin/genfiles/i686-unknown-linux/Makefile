include ../../../Makefile.inc

CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib
CYC_BIN_PATH=$(CYCDIR)/bin

.PHONY: install compile

install: install_lib install_comp
install_a: comp_a $(CYC_LIB_PATH)/libcyc_a.a
install_lib: comp_lib $(CYC_LIB_PATH)/$(CYCLIB) 
install_comp: comp_src $(CYC_BIN_PATH)/cyclone$(EXE)

comp_lib:
	$(MAKE) -C lib
comp_src:
	$(MAKE) -C src
comp_a:
	$(MAKE) -C lib libcyc_a.a nogc_a.a

$(CYC_LIB_PATH)/$(CYCLIB): lib/$(CYCLIB)
	cp $< $@
	cp lib/nogc.a $(CYC_LIB_PATH)

$(CYC_BIN_PATH)/cyclone$(EXE): src/cyclone$(EXE)
	cp $< $@

$(CYC_LIB_PATH)/libcyc_a.a: lib/libcyc_a.a
	cp $< $@
	cp lib/nogc_a.a $(CYC_LIB_PATH)

uninstall: clean
	$(RM) $(CYC_LIB_PATH)/$(CYCLIB)
	$(RM) $(CYC_LIB_PATH)/nogc.a
	$(RM) $(CYC_BIN_PATH)/cyclone$(EXE)
	$(RM) $(CYC_LIB_PATH)/cyclib_a.a
	$(RM) $(CYC_LIB_PATH)/nogc_a.a

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
