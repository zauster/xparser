all: dvi ps

spell:
	ispell user_manual.tex

dvi:
	latex user_manual
	bibtex user_manual
	latex user_manual
	latex user_manual

ps: dvi
	dvips -t a4 user_manual.dvi -o user_manual.ps

clean:
	rm -f *.aux user_manual.bbl user_manual.blg user_manual.dvi user_manual.log user_manual.lof user_manual.lot user_manual.toc *.ps
