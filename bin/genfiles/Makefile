
include ../../Makefile.inc

.PHONY: install compile

install: compile copy

compile:
	$(MAKE) -C lib
	$(MAKE) -C src

copy: ../cyc-lib/$(CYCLIB) ../cyclone$(EXE) ../cyc-lib/nogc.a

../cyc-lib/$(CYCLIB): lib/$(CYCLIB)
	cp lib/$(CYCLIB) ../cyc-lib

../cyclone$(EXE): src/cyclone$(EXE)
	cp src/cyclone$(EXE) ..

../cyc-lib/nogc.a: lib/nogc.a
	cp lib/nogc.a ../cyc-lib

install_a:
	$(MAKE) -C lib libcyc_a.a nogc_a.a
	cp lib/libcyc_a.a ../cyc-lib
	cp lib/nogc_a.a ../cyc-lib

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
