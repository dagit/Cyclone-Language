
include ../../Makefile.inc

install:
	$(MAKE) -C lib
	$(MAKE) -C src
	cp lib/$(CYCLIB) ..
	cp src/cyclone.exe ..

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C lib
