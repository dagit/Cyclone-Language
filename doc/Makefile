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

%.html: %.tex gen-libdoc.tex
	hevea $<
	hevea $<

%.pdf: %.tex gen-libdoc.tex
	pdflatex '\nonstopmode\input' $<
	makeindex $<
	pdflatex '\nonstopmode\input' $<
	pdflatex '\nonstopmode\input' $<

%.dvi: %.tex gen-libdoc.tex
	latex '\nonstopmode\input' $<
	makeindex $<
	latex '\nonstopmode\input' $<
	latex '\nonstopmode\input' $<

%.ps: %.dvi
	dvips -o $@ $<

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
	$(RM) *.pdf *.ps *.toc *.out *~
	$(RM) main-screen.html
	$(RM) gen-libdoc.tex
	$(RM) -r online-manual

#.PHONY: main-screen.html main-screen.pdf main-letter.pdf main-a4.pdf online-manual
