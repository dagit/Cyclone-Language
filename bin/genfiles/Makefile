
include ../../Makefile.inc

.PHONY: install compile

install: compile copy

compile:
	$(MAKE) -C lib
	$(MAKE) -C src

copy: ../$(CYCLIB) ../cyclone.exe

../$(CYCLIB): lib/$(CYCLIB)
	cp lib/$(CYCLIB) ../cyc-lib

../cyclone.exe: src/cyclone.exe
	cp src/cyclone.exe ..

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
