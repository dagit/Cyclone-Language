
include ../../Makefile.inc

.PHONY: install compile

install: compile copy

compile:
	$(MAKE) -C lib
	$(MAKE) -C src

copy: ../cyc-lib/$(CYCLIB) ../cyclone$(EXE)

../cyc-lib/$(CYCLIB): lib/$(CYCLIB)
	cp lib/$(CYCLIB) ../cyc-lib

../cyclone$(EXE): src/cyclone$(EXE)
	cp src/cyclone$(EXE) ..

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
