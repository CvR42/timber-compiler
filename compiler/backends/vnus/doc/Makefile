MAKEFILE = Makefile

COMPILERSRCROOT=../../..

DISTFILES = $(MAKEFILE) error-list

help:
	@echo " Possible make targets:"
	@echo "all              Create local running programs."
	@echo "clean            Delete all temporary files."
	@echo "empty            Delete all temporary files and executables."
	@echo "help             This help text."

include $(COMPILERSRCROOT)/Makefiles/Make.dist

empty:
	rm -f $(JUNK)

clean:
	rm -f $(JUNK)

