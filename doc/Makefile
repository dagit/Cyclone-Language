all: library.info library.html

library.info:
	makeinfo --no-split library.texinfo

library.html:
	makeinfo --html library.texinfo

clean:
	$(RM) -rf library.info library.html
