JUNK = refman.aux refman.dvi refman.log refman.ps \
       implementation.aux implementation.dvi implementation.log \
       implementation.ps

COMPILERSRCROOT = ../../..

MAKEFILE = Makefile

DISTFILES = $(MAKEFILE) error-list

all: refman.dvi implementation.dvi

refman.dvi: refman.tex
	latex $<

implementation.dvi: implementation.tex
	latex $<

refman.ps: refman.dvi
	dvips $<

implementation.ps: implementation.dvi
	dvips $<

help:
	@echo " Possible make targets:"
	@echo "all              Create local running programs."
	@echo "clean            Delete all temporary files."
	@echo "empty            Delete all temporary files and executables."
	@echo "help             This help text."

empty: clean

include $(COMPILERSRCROOT)/Makefiles/Make.dist

clean:
	rm -f $(JUNK)

