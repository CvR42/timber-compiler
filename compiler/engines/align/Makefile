# Top-level makefile for the align directory.



include ../mapper/Make.vars

TMROOT=/Users/reeuwijk/lab

prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
UNINSTALL_PROGRAM = rm -f
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "reallyempty      Delete all generated files and links."
	@$(ECHO) "depend           Update all dependencies files"
	@$(ECHO) "symbolic         Make symbolic links to files in backend."

all: symbolic
	$(MAKE) -f Make.align all

clean:
	$(MAKE) -f Make.align clean

tests:
	@$(ECHO) "No tests implemented"

check:
	@$(ECHO) "No tests implemented"

install: align
	$(INSTALL) -d ${exec_prefix}/libexec
	$(INSTALL_PROGRAM) align ${exec_prefix}/libexec/spar-align

uninstall: align
	$(UNINSTALL_PROGRAM) ${exec_prefix}/libexec/spar-align

empty:
	$(MAKE) -f Make.align empty

depend: symbolic
	$(MAKE) -f Make.align depend

symbolic: defs.h

defs.h:	$(VNUSDIR)/defs.h
	ln -s $(VNUSDIR)/defs.h
	ln -s $(VNUSDIR)/error.c error.cc
	ln -s $(VNUSDIR)/error.h
	ln -s $(VNUSDIR)/tmadmin.ct

SOFTLINKS	= defs.h error.cc error.h tmadmin.ct 

TMSRC = tmadmin.ds tmadmin.t tmadmin.ht hoist_on.ct

SRC = align.cc global.cc main.cc
HDR = global.h service.h hoist_on.h

CONFIGUREFILES = Makefile.in configure configure.ac
DISTFILES = $(CONFIGUREFILES) $(TMSRC) $(SRC) $(HDR) .cvsignore Make.align


reallyempty:	empty
	rm -f $(SOFTLINKS)

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist

