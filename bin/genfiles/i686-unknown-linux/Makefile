include ../../../Makefile.inc

CYC_LIB_PATH=$(CYCDIR)/bin/cyc-lib
CYC_BIN_PATH=$(CYCDIR)/bin

.PHONY: install compile

install: install_lib install_comp
install_a: compile_a copy_a
install_lib: compile_lib copy_lib
install_comp: compile_comp copy_comp

compile_lib:
	$(MAKE) -C lib

copy_lib:
	cp lib/$(CYCLIB) $(CYC_LIB_PATH)
	cp lib/nogc.a $(CYC_LIB_PATH)
	
compile_comp:
	$(MAKE) -C src

copy_comp:
	cp src/cyclone$(EXE) $(CYC_BIN_PATH)
	
compile_a:
	$(MAKE) -C lib libcyc_a.a nogc_a.a

copy_a: 
	cp lib/libcyc_a.a $(CYC_LIB_PATH)
	cp lib/nogc_a.a $(CYC_LIB_PATH)

uninstall: clean
	$(RM) $(CYC_LIB_PATH)/$(CYCLIB)
	$(RM) $(CYC_LIB_PATH)/nogc.a
	$(RM) $(CYC_BIN_PATH)/cyclone$(EXE)
	$(RM) $(CYC_LIB_PATH)/cyclib_a.a
	$(RM) $(CYC_LIB_PATH)/nogc_a.a

#$(CYC_LIB_PATH)/$(CYCLIB): lib/$(CYCLIB)
#	cp lib/$(CYCLIB) $(CYC_LIB_PATH)
#
#$(CYC_BIN_PATH)/cyclone$(EXE): src/cyclone$(EXE)
#	cp src/cyclone$(EXE) $(CYC_BIN_PATH)
#
#$(CYC_LIB_PATH)/nogc.a: lib/nogc.a
#	cp lib/nogc.a $(CYC_LIB_PATH)
#

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
