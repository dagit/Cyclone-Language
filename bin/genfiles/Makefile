
include ../../Makefile.inc

.PHONY: install compile

install: compile copy

compile:
	$(MAKE) -C lib
	$(MAKE) -C src

copy: ../$(CYCLIB) ../cyclone$(EXE)

../$(CYCLIB): lib/$(CYCLIB)
	cp lib/$(CYCLIB) ../cyc-lib

../cyclone$(EXE): src/cyclone$(EXE)
	cp src/cyclone$(EXE) ..

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
