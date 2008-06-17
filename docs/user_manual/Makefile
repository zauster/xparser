all: dvi ps

swarm_1.ps: swarm_1.dot
	dot -Tps swarm_1.dot -o swarm_1.ps

swarm_2.ps: swarm_2.dot
	dot -Tps swarm_2.dot -o swarm_2.ps

swarm_3.ps: swarm_3.dot
	dot -Tps swarm_3.dot -o swarm_3.ps

spell:
	aspell user_manual.tex
	aspell model_design.tex
	apsell model_description.tex
	aspell model_implementation.tex
	apsell model_execution.tex

images: swarm_1.ps swarm_2.ps swarm_3.ps

dvi: images
	#latex user_manual
	#bibtex user_manual
	latex user_manual
	latex user_manual

ps: dvi
	dvips -t a4 user_manual.dvi -o user_manual.ps

clean:
	rm -f *.aux user_manual.bbl user_manual.blg user_manual.dvi user_manual.log user_manual.lof user_manual.lot user_manual.toc *.ps

vclean: clean
	rm -f *.bak