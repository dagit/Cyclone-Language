all: library.info library.html

library.info: library.texinfo
	makeinfo --no-split library.texinfo

library.html: library.texinfo
	makeinfo --html library.texinfo

clean:
	$(RM) -rf library.info library.html
