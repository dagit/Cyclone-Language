# doc/Makefile

# read this just to get VERSION
include ../Makefile.inc

CYCDOC=../bin/cycdoc
INPUTS= advanced.tex \
	aprof.tex \
	buildlib.tex \
	compiler.tex \
	definiteassignment.tex \
	faq.tex \
	gen-libdoc.tex \
	generics.tex \
	grammar.tex \
	installing.tex \
	intro.tex \
	libraries.tex \
	main-stub.tex \
	namespaces.tex \
	patterns.tex \
	pointers.tex \
	porting.tex \
	regions.tex \
	taggedunions.tex \
	tutorial.tex \
	typeinference.tex \
	varargs.tex

all: screen html

html: main-screen.html

screen: main-screen.pdf

letter: main-letter.pdf

a4: main-a4.pdf

tarball: online-manual letter
	tar cvf cyclone-$(VERSION)-docs.tar online-manual main-letter.pdf
	gzip cyclone-$(VERSION)-docs.tar

online-manual: main-screen.html
	$(RM) -r online-manual
	mkdir online-manual
	cp *.html online-manual
	(cd online-manual; hacha main-screen.html)

%.html: %.tex $(INPUTS)
	hevea $<
	hevea $<

%.pdf: %.tex $(INPUTS)
	pdflatex '\nonstopmode\input' $<
	makeindex $<
	pdflatex '\nonstopmode\input' $<
	pdflatex '\nonstopmode\input' $<

%.dvi: %.tex $(INPUTS)
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
