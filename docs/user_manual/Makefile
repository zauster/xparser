all: dvi ps

swarm_1.eps: swarm_1.dot
	dot -Teps swarm_1.dot -o swarm_1.eps

swarm_2.eps: swarm_2.dot
	dot -Teps swarm_2.dot -o swarm_2.eps

swarm_3.eps: swarm_3.dot
	dot -Teps swarm_3.dot -o swarm_3.eps

iteration_1.eps: iteration_1.dot
	dot -Teps iteration_1.dot -o iteration_1.eps

spell:
	aspell -c user_manual.tex
	aspell -c model_design.tex
	aspell -c model_description.tex
	aspell -c model_implementation.tex
	aspell -c model_execution.tex

images: swarm_1.eps swarm_2.eps swarm_3.eps iteration_1.eps

png: iteration_1.png swarm_2.png

dvi: images
	#latex user_manual
	#bibtex user_manual
	latex user_manual
	latex user_manual

ps: dvi
	dvips -t a4 user_manual.dvi -o user_manual.ps

pdf: dvi
	dvipdf user_manual.dvi user_manual.pdf

html:
	latex2html -nonavigation -noinfo -split 0 user_manual.tex

overview:
	latex overview
	dvipdf overview.dvi overview.pdf

clean:
	rm -f *.aux user_manual.bbl user_manual.blg *.dvi *.log user_manual.lof user_manual.lot user_manual.toc *.ps *.pdf swarm_1.eps swarm_2.eps swarm_3.eps iteration_1.eps
	rm -R user_manual

vclean: clean
	rm -f *.bak