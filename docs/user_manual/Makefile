all: dvi ps

swarm_1.ps: swarm_1.dot
	dot -Tps swarm_1.dot -o swarm_1.ps

swarm_2.ps: swarm_2.dot
	dot -Tps swarm_2.dot -o swarm_2.ps

swarm_3.ps: swarm_3.dot
	dot -Tps swarm_3.dot -o swarm_3.ps

iteration_1.ps: iteration_1.dot
	dot -Tps iteration_1.dot -o iteration_1.ps

spell:
	aspell -c user_manual.tex
	aspell -c model_design.tex
	aspell -c model_description.tex
	aspell -c model_implementation.tex
	aspell -c model_execution.tex

images: swarm_1.ps swarm_2.ps swarm_3.ps iteration_1.ps

dvi: images
	#latex user_manual
	#bibtex user_manual
	latex user_manual
	latex user_manual

ps: dvi
	dvips -t a4 user_manual.dvi -o user_manual.ps

pdf: dvi
	dvipdf user_manual.dvi user_manual.pdf

clean:
	rm -f *.aux user_manual.bbl user_manual.blg user_manual.dvi *.log user_manual.lof user_manual.lot user_manual.toc *.ps *.pdf

vclean: clean
	rm -f *.bak