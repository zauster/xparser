all: dvi ps

spell:
	aspell developer_manual.tex

dvi:
	#latex developer_manual
	#bibtex developer_manual
	latex developer_manual
	latex developer_manual

ps: dvi
	dvips -t a4 developer_manual.dvi -o developer_manual.ps

clean:
	rm -f *.aux developer_manual.bbl developer_manual.blg developer_manual.dvi developer_manual.log developer_manual.lof developer_manual.lot developer_manual.toc *.ps
