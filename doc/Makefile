all: screen html

html: main-screen.html

screen: main-screen.pdf

letter: main-letter.pdf

a4: main-a4.pdf

main-screen.html:
	hevea main-screen
	hevea main-screen

main-screen.pdf:
	pdflatex '\nonstopmode\input' main-screen
	makeindex main-screen
	pdflatex '\nonstopmode\input' main-screen
	pdflatex '\nonstopmode\input' main-screen

main-letter.pdf:
	pdflatex '\nonstopmode\input' main-letter
	makeindex main-letter
	pdflatex '\nonstopmode\input' main-letter
	pdflatex '\nonstopmode\input' main-letter


main-a4.pdf:
	pdflatex '\nonstopmode\input' main-a4
	makeindex main-a4
	pdflatex '\nonstopmode\input' main-a4
	pdflatex '\nonstopmode\input' main-a4

clean:
	$(RM) *.aux *.dvi *.hind *.haux *.idx *.ilg *.ind *.ing *.log
	$(RM) *.pdf *.toc *.out
	$(RM) main-screen.html

.PHONY: main-screen.html main-screen.pdf main-letter.pdf main-a4.pdf
