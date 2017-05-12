# Top-level makefile.



COMPILERSRCROOT = ..

JUNK =
PRECIOUSJUNK = 

help:
	@echo "TIMBER ENGINES -- Possible make targets:"
	@echo "--------------------------------------------------------"
	@echo "all          Compile all targets"
	@echo "tests        Run all Vnus test programs."
	@echo "clean        Delete all temporary files."
	@echo "empty        Make everything spotless."
	@echo "depend       Update all dependencies."
	@echo "install      Install software (prefix=/Users/reeuwijk/lab)."
	@echo "--------------------------------------------------------"

all:
	(cd mapper && $(MAKE) all)
	(cd align && $(MAKE) all)

tests:
	@(cd mapper && $(MAKE) tests && \
	  cd ../align && $(MAKE) tests )

check:
	@(cd mapper && $(MAKE) tests && \
	  cd ../align && $(MAKE) tests )

clean:
	rm -f $(JUNK)
	(cd mapper && $(MAKE) clean)
	(cd align && $(MAKE) clean)

empty:
	rm -f $(JUNK) $(PRECIOUSJUNK)
	(cd mapper && $(MAKE) reallyempty)
	(cd align && $(MAKE) reallyempty)

depend:
	(cd mapper && $(MAKE) depend)
	(cd align && $(MAKE) depend)

install:
	(cd mapper && $(MAKE) install)
	(cd align && $(MAKE) install)

uninstall:
	(cd mapper && $(MAKE) uninstall)
	(cd align && $(MAKE) uninstall)

DISTFILES = Makefile.in configure configure.ac install-sh .cvsignore README

SUBDIRS = align mapper

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist
