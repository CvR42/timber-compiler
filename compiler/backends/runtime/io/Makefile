# Makefile for the V-nus library functions.

COMPILERSRCROOT=../../..

prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
UNINSTALL_DATA = rm -f
INSTALL_INCLUDEDIR = ${prefix}/include/timber
RANLIB = ranlib

#If LIBBASE is defined we get the make rules for libraries for free:-)
LIBBASE	= vnusio
# Note that contrary to all other places the P here means parallel, not Panda.
# All code in this directory is compiled with the sequential flags.
LIB_S	= $(LIBBASE:%=../lib%-s.a)
LIB_P	= $(LIBBASE:%=../lib%-p.a)
LIB_TS	= $(LIBBASE:%=../lib%-ts.a)
LIB_TP	= $(LIBBASE:%=../lib%-tp.a)

ifeq ($(HAVE_TRIMEDIA), yes)
LIB_TSC		= $(LIB_TS)
LIB_TPC		= $(LIB_TP)
else
LIB_TSC		=
LIB_TPC		=
endif

SRC_SEQ	= ioseq.cc
SRC_PAR	= iopar.cc
SRC_S	= $(SRC_SEQ)
SRC_P	= $(SRC_PAR)
SRC_TS	= $(SRC_SEQ)
SRC_TP	= $(SRC_PAR)

OBJ_S	    = $(SRC_S:%.cc=%-s.o)
OBJ_P	    = $(SRC_P:%.cc=%-s.o)
OBJ_TS	    = $(SRC_TS:%.cc=%-ts.o)
OBJ_TP	    = $(SRC_TP:%.cc=%-ts.o)

GEN_H	= ioseq.h iopar.h
GEN_C	= ioseq.cc iopar.cc

ECHO    = echo

JUNK	= dist.lst

TM_SRC = $(GEN_H:.h=.ht) $(GEN_C:.cc=.cct)

HDR = 

OTHER_SRC = 

DISTFILES = .cvsignore Makefile.in configure configure.ac $(SRC) $(OTHER_SRC) $(HDR) $(TM_SRC)

all:		depend targets

# The magical stuff
include $(COMPILERSRCROOT)/Makefiles/Make.vars
#
#EXTRAFLAGS += -I/usr/include/lam
EXTRAFLAGS += -g
#EXTRAFLAGS += -pg
#EXTRAFLAGS += -save-temps
#EXTRAFLAGS += -DDEBUG_VnusCommunication

CXX = g++
CXXFLAGS = -g -O2
BUILDDEPENDS = g++ -MM

targets:	$(LIB_S) $(LIB_P) $(LIB_TSC) $(LIB_TPC)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "install          Install the software."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."

touch:
	touch $(SRC) 

clean:
	rm -f $(OBJ_S) $(OBJ_P) $(OBJ_TS) $(OBJ_TP) $(JUNK) $(GEN_H) $(GEN_C)

empty: 	clean
	rm -f $(LIB_S) $(LIB_P) $(LIB_TS) $(LIB_TP) $(DEPENDFILE)

depend:	$(DEPENDFILE)

DEPENDSRC = $(SRC_SEQ) $(SRC_PAR)

$(DEPENDFILE): $(DEPENDSRC) $(GEN_H:.ht=.h)

install: $(LIB_S)
	$(INSTALL) -d ${exec_prefix}/lib
	$(INSTALL) -d ${prefix}/include
	$(INSTALL) -d $(INSTALL_INCLUDEDIR)
	$(INSTALL_DATA) $(LIB_S) ${exec_prefix}/lib/lib$(LIBBASE)-s.a
	$(RANLIB) ${exec_prefix}/lib/lib$(LIBBASE)-s.a
	$(INSTALL_DATA) $(LIB_P) ${exec_prefix}/lib/lib$(LIBBASE)-p.a
	$(RANLIB) ${exec_prefix}/lib/lib$(LIBBASE)-p.a

uninstall: 
	$(UNINSTALL_DATA) ${exec_prefix}/lib/lib$(LIBBASE)-s.a
	$(UNINSTALL_DATA) ${exec_prefix}/lib/lib$(LIBBASE)-p.a

headers: $(GEN_H:.ht=.h)
	
%.h: %.ht ../lib.ds ../lib.t
	tm ../lib.ds $< > $@

%.cc: %.cct ../lib.ds ../lib.t
	tm ../lib.ds $< > $@

stats:
	wc -l *.cc *.h

genstats: $(GEN_C) $(GEN_H)
	wc -l $(GEN_C) $(GEN_H)


$(LIB_S):	$(OBJ_S:%.o=$(LIB_S)(%.o))
	ranlib $@
	@rm -f $(OBJ_S)

$(LIB_P):	$(OBJ_P:%.o=$(LIB_P)(%.o))
	ranlib $@
	@rm -f $(OBJ_P)

$(LIB_TS):	$(OBJ_TS:%.o=$(LIB_TS)(%.o))
	@rm -f $(OBJ_TS)

$(LIB_TP):	$(OBJ_TP:%.o=$(LIB_TP)(%.o))
	@rm -f $(OBJ_TP)

$(LIB_S)(%-s.o):	%.cc
	$(CXX) $(CXXSFLAGS) -c $< -o $%
	$(AR) $(ARFLAGS) $(LIB_S) $%

$(LIB_P)(%-s.o):	%.cc
	$(CXX) $(CXXSFLAGS) -c $< -o $%
	$(AR) $(ARFLAGS) $(LIB_P) $%

$(LIB_TS)(%-ts.o):	%.cc
	$(TMCXX) $(TMCXXSFLAGS) -c $< -o $%
	$(TMAR) $(TMARFLAGS) $(LIB_TS) $%

$(LIB_TP)(%-ts.o):	%.cc
	$(TMCXX) $(TMCXXSFLAGS) -c $< -o $%
	$(TMAR) $(TMARFLAGS) $(LIB_TP) $%

$(DEPENDFILE):
	$(BUILDDEPENDS) $(CXXSFLAGS)  $(SRC_S)  | sed "s#\(.*\)\.o#$(LIB_S)(\1-s\.o)#g"   >  $(DEPENDFILE)
	$(BUILDDEPENDS) $(CXXSFLAGS)  $(SRC_P)  | sed "s#\(.*\)\.o#$(LIB_P)(\1-s\.o)#g"   >>  $(DEPENDFILE)
ifeq ($(HAVE_TRIMEDIA), yes)
	$(BUILDDEPENDS) $(TMCXXSFLAGS) $(SRC_TS) | sed "s#\(.*\)\.o#$(LIB_TS)(\1-ts\.o)#g" >> $(DEPENDFILE)
	$(BUILDDEPENDS) $(TMCXXSFLAGS) $(SRC_TP) | sed "s#\(.*\)\.o#$(LIB_TP)(\1-ts\.o)#g" >> $(DEPENDFILE)
endif

SUBDIRS =

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist

-include $(DEPENDFILE)
