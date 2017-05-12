MAKEFILE = Makefile



COMPILERSRCROOT=../..

# All this stuff is filled in by configure
prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

DISTFILES = .cvsignore install-sh Makefile.in configure configure.ac $(wildcard *.java *.spar)

SUBDIRS =  java kaffe spar

JUNK = dist.lst
PRECIOUSJUNK =
MYPATH = ${exec_prefix}/lib/sparlib

include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.std

all:
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) all && cd ..; done
endif

check:
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) check && cd ..; done
endif

tests:
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) tests && cd ..; done
endif

install:
	rm -rf $(MYPATH)
	$(INSTALL) -d $(MYPATH)
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) install && cd ..; done
endif

uninstall:
	rm -rf $(MYPATH)
