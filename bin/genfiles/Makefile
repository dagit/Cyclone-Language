
include ../../Makefile.inc

.PHONY: install compile

install: compile copy

compile:
	$(MAKE) -C lib
	$(MAKE) -C src

copy: ../$(CYCLIB) ../cyclone$(EXE)

../$(CYCLIB): lib/$(CYCLIB)
	cp lib/$(CYCLIB) ../cyc-lib

#JGM:  I had to put the .exe on here to get this to work under Win32
#hopefully someone will be able to fix this...
../cyclone$(EXE): src/cyclone$(EXE)
	cp src/cyclone.exe ..

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
