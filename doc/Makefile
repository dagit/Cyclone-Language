all: screen html

html: main-screen.html

main-screen.html:
	hevea main-screen.tex

screen: main-screen.pdf

main-screen.pdf:
	pdflatex '\nonstopmode\input' main-screen
	makeindex main-screen
	pdflatex '\nonstopmode\input' main-screen
	pdflatex '\nonstopmode\input' main-screen

main-letter.pdf:
	pdflatex '\nonstopmode\input' main-letter
	makeindex main-screen
	pdflatex '\nonstopmode\input' main-letter
	pdflatex '\nonstopmode\input' main-letter

clean:
	$(RM) *.aux *.dvi *.hind *.idx *.ilg *.ind *.ing *.log
	$(RM) *.pdf *.toc *.out
	$(RM) main-screen.html
