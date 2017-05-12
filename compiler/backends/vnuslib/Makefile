# Makefile for the functions used in the compiler backends.



COMPILERSRCROOT=../..
TMROOT=/Users/reeuwijk/lab

# All this stuff is filled in by configure
prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
UNINSTALL_DATA = rm -f
INSTALL_INCLUDEDIR = ${prefix}/include/timber

#If LIBBASE is defined we get the make rules for libraries for free :-)
LIBBASE	= vnusctl
LIB	= $(LIBBASE:%=lib%.a)
HANDSRC = service.cc typederive.cc symtab.cc error.cc analyze.cc x2l.cc \
      vnusbasetypes.cc equivalent.cc

OBJ	= $(SRC:%.cc=%.o)

GEN_H	= tmadmin.h

CTFILES = constfold.ct collectscopes.ct

LD = $(CXX)
BUILDDEPENDS = g++ -MM

GEN_C	= $(CTFILES:.ct=.cc)

SRC = $(HANDSRC) $(GEN_C)

ECHO    = echo

JUNK	= 

VNUSDS = $(COMPILERSRCROOT)/backends/vnuslib/vnus.ds

# Non-generated header files
HDRS = vnusbasetypes.h vnusctl.h collectscopes.h

TMFILES = vnus.ds tmadmin.t tmadmin.ht $(CTFILES)

CONFIGFILES = config.h.in Makefile.in configure configure.ac

DISTFILES = $(CONFIGFILES) $(HANDSRC) $(HDRS) $(TMFILES)

JUNK += $(OBJ) $(GEN_H) $(GEN_C)
PRECIOUSJUNK += $(LIB) $(DEPENDFILE) dist.lst

all:		depend targets

# The magical stuff
include $(COMPILERSRCROOT)/Makefiles/Make.vars
#
EXTRAFLAGS += -g

#EXTRAFLAGS += -DTMC_CHECKED_CAST

#EXTRAFLAGS += -DLOGNEW

#EXTRAFLAGS += -DDEBUG_VnusCommunication

targets:	$(LIB)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."
	@$(ECHO) "tests            Test the library."
	@$(ECHO) "install          Install the library."

tests:

check:

touch:
	touch $(SRC) 

depend:	$(DEPENDFILE)

$(DEPENDFILE): $(SRC) $(GEN_H) $(HDRS)
	$(BUILDDEPENDS) $(CXXSFLAGS) $(SRC) | sed "s#\(.*\)\.o#$(LIB)(\1\.o)#g" > $(DEPENDFILE)

%.h: %.ht $(VNUSDS) %.t
	tm $(VNUSDS) $< > $@

constfold.cc: constfold.ct $(VNUSDS)
	tm $(VNUSDS) $< > $@

collectscopes.cc: collectscopes.ct $(VNUSDS)
	tm $(VNUSDS) $< > $@

$(LIB): $(OBJ:%.o=$(LIB)(%.o))
	ranlib $@
	@rm -f $(OBJ)

$(LIB)(%.o): %.cc
	$(CXX) $(CXXSFLAGS) -c $< -o $%
	$(AR) $(ARFLAGS) $(LIB) $%

stats:
	wc -l *.cc *.h

genstats: $(GEN_C) $(GEN_H)
	wc -l $(GEN_C) $(GEN_H)

install: $(LIB) vnusctl.h vnusbasetypes.h
	$(INSTALL) -d ${exec_prefix}/lib
	$(INSTALL) -d ${prefix}/include
	$(INSTALL) -d $(INSTALL_INCLUDEDIR)
	$(INSTALL_DATA) $(LIB) ${exec_prefix}/lib/$(LIB)
	$(RANLIB) ${exec_prefix}/lib/$(LIB)
	$(INSTALL_DATA) vnusctl.h $(INSTALL_INCLUDEDIR)/vnusctl.h
	$(INSTALL_DATA) vnusbasetypes.h $(INSTALL_INCLUDEDIR)/vnusbasetypes.h

uninstall: 
	$(UNINSTALL_DATA) ${exec_prefix}/lib/$(LIB)
	$(UNINSTALL_DATA) $(INSTALL_INCLUDEDIR)/vnusctl.h
	$(UNINSTALL_DATA) $(INSTALL_INCLUDEDIR)/vnusbasetypes.h

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.std

-include $(DEPENDFILE)
