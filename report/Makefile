FILE = main

.PHONY: $(FILE).pdf all clean

all: $(FILE).pdf

$(FILE).pdf: $(FILE).tex
	pdflatex $<

clean:
	rm -rf *.aux *.fdb_latexmk *.fls *.log *.out
