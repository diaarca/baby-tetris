FILE = slides

.PHONY: $(FILE).pdf all clean distclean

all: $(FILE).pdf

$(FILE).pdf: $(FILE).tex
	pdflatex $<
	bibtex $<
	pdflatex $<
	pdflatex $<

clean:
	rm -rf *.aux *.fdb_latexmk *.fls *.log *.out

distclean: clean
	rm $(FILE).tex
