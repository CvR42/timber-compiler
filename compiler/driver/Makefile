

COMPILERSRCROOT = ..

JUNK = dist.lst

prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
SPARDRIVER = spar
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
UNINSTALL_PROGRAM = rm -f

INSTALLED_SPARDRIVER = spar-installed
SPARPREAMBLE = ../Makefiles/spar-preamble

DRIVERSCRIPTS = spar-runner.sh runner-develop-config.sh.in runner-install-config.sh.in

DISTFILES = Makefile.in $(DRIVERSCRIPTS) configure configure.ac

JUNK += runner-develop.config.sh runner-install-config.sh

help:
	@echo "TIMBER driver script -- Possible make targets:"
	@echo "--------------------------------------------------------"
	@echo "all          Compile all targets"
	@echo "tests        Run all Spar test programs."
	@echo "clean        Delete all temporary files."
	@echo "empty        Make everything spotless."
	@echo "depend       Update all dependencies."
	@echo "install      Install the software."
	@echo "--------------------------------------------------------"

all: $(INSTALLED_SPARDRIVER) $(SPARDRIVER)

check:

tests:

install: $(INSTALLED_SPARDRIVER)
	$(INSTALL_PROGRAM) $(INSTALLED_SPARDRIVER) ${exec_prefix}/bin/$(SPARDRIVER)

uninstall:
	$(UNINSTALL_PROGRAM) ${exec_prefix}/bin/$(SPARDRIVER)

depend:


$(SPARDRIVER): runner-develop-config.sh $(SPARPREAMBLE) spar-runner.sh
	cat runner-develop-config.sh $(SPARPREAMBLE) spar-runner.sh > $@
	chmod +x $@

$(INSTALLED_SPARDRIVER): runner-install-config.sh $(SPARPREAMBLE) spar-runner.sh
	cat runner-install-config.sh $(SPARPREAMBLE) spar-runner.sh > $@
	chmod +x $@


.PHONY: help all check tests install clean empty depend

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.std
