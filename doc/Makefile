CYCDOC=../bin/cycdoc

all: screen html

html: main-screen.html

screen: main-screen.pdf

letter: main-letter.pdf

a4: main-a4.pdf

tarball: online-manual letter
	@(VERSION=`cat ../.version`; \
	  tar cvf cyclone-$VERSION-docs.tar online-manual main-letter.pdf
	  gzip cyclone-$VERSION-docs.tar)

online-manual: main-screen.html gen-libdoc.tex
	$(RM) -r online-manual
	mkdir online-manual
	cp *.html online-manual
	(cd online-manual; hacha main-screen.html)

main-screen.html: gen-libdoc.tex
	hevea main-screen
	hevea main-screen

main-screen.pdf: gen-libdoc.tex
	pdflatex '\nonstopmode\input' main-screen
	makeindex main-screen
	pdflatex '\nonstopmode\input' main-screen
	pdflatex '\nonstopmode\input' main-screen

main-letter.pdf: gen-libdoc.tex
	pdflatex '\nonstopmode\input' main-letter
	makeindex main-letter
	pdflatex '\nonstopmode\input' main-letter
	pdflatex '\nonstopmode\input' main-letter

main-a4.pdf: gen-libdoc.tex
	pdflatex '\nonstopmode\input' main-a4
	makeindex main-a4
	pdflatex '\nonstopmode\input' main-a4
	pdflatex '\nonstopmode\input' main-a4

gen-libdoc.tex: \
  ../include/array.h \
  ../include/bitvec.h \
  ../include/buffer.h \
  ../include/core.h \
  ../include/dict.h \
  ../include/filename.h \
  ../include/fn.h \
  ../include/hashtable.h \
  ../include/list.h \
  ../include/marshal.h \
  ../include/pp.h \
  ../include/queue.h \
  ../include/rope.h \
  ../include/set.h \
  ../include/slowdict.h \
  ../include/xarray.h
	$(CYCDOC) -cyclone ../bin/cyclone -I../include -B../bin/lib/cyc-lib $^ > gen-libdoc.tex

clean:
	$(RM) *.aux *.dvi *.hind *.haux *.idx *.ilg *.ind *.ing *.log
	$(RM) *.pdf *.toc *.out *~
	$(RM) main-screen.html
	$(RM) gen-libdoc.tex
	$(RM) -r online-manual

.PHONY: main-screen.html main-screen.pdf main-letter.pdf main-a4.pdf online-manual
