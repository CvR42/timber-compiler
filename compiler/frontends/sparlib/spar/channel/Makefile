MAKEFILE = Makefile



COMPILERSRCROOT=../../../..

# All this stuff is filled in by configure
prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

SRCFILES = $(wildcard *.java *.spar)
DISTFILES = Makefile.in configure configure.ac $(SRCFILES)

SUBDIRS =

JUNK = dist.lst
PRECIOUSJUNK =
MYPATH = ${exec_prefix}/lib/sparlib/spar/compiler

include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.configure

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

clean:
	rm -f $(JUNK)
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) clean && cd ..; done
endif

empty:
	rm -f $(JUNK) $(PRECIOUSJUNK)
ifdef SUBDIRS
	for i in $(SUBDIRS); do cd $$i && $(MAKE) empty && cd ..; done
endif

install:
	$(INSTALL) -d $(MYPATH)
	for i in $(SRCFILES); do $(INSTALL_DATA) $$i $(MYPATH)/$$i; done

