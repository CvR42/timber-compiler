# Ultra-simpele makefile voor de reference manual.

JUNK = refman.aux refman.dvi refman.log refman.ps

all: refman.dvi

refman.dvi: refman.tex
	latex $<

refman.ps: refman.dvi
	dvips refman.dvi

help:
	@echo " Possible make targets:"
	@echo "all              Create local running programs."
	@echo "clean            Delete all temporary files."
	@echo "empty            Delete all temporary files and executables."
	@echo "help             This help text."

empty: clean

clean:
	rm -f $(JUNK)

