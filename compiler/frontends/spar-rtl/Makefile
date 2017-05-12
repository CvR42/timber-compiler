# Makefile for the Spar run-time library functions.

COMPILERSRCROOT=../..



# All this stuff is filled in by configure
prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
UNINSTALL_DATA = rm -f
INSTALL_INCLUDEDIR = ${prefix}/include/timber
RANLIB = ranlib

#If LIBBASE is defined we get the make rules for libraries for free :-)
LIBBASE	= spar-rtl
LIB_S	= $(LIBBASE:%=lib%-s.a)
LIB_TS	= $(LIBBASE:%=lib%-ts.a)


ALLLIBS = $(LIB_S) $(LIB_TSC)

SRC = currentTimeMillis.cc initProperties.cc unicodify.cc fileexists.cc \
    fillInStackTrace.cc write.cc Introspection.cc getprim.cc getfilestring.cc \
    gcelements.cc d2l.cc l2d.cc i2f.cc f2i.cc dfmt.cc argv.cc read.cc open.cc \
    close.cc skip.cc available.cc valueofdouble.cc math-rint.cc isFile.cc \
    filereadable.cc filewriteable.cc serialisation.cc getclassstring.cc \
    isDirectory.cc

SRC_S	= $(SRC)
SRC_TS	= $(SRC)

OBJ_S	    = $(SRC_S:%.cc=%-s.o)
OBJ_TS	    = $(SRC_TS:%.cc=%-ts.o)
CTEST	    = ctests.c
ECHO    = echo

JUNK	= $(CTEST:%.c=%.o) testout $(ALLLIBS) $(OBJ_S) $(OBJ_TS) $(OBJ_T) $(OBJ_D) $(OBJ_M) $(OBJ_P) $(OBJ_E) $(OBJ_TE)

PRECIOUSJUNK = $(ALLLIBS) $(DEPENDFILE)

all:		depend targets

# The magical stuff
include $(COMPILERSRCROOT)/Makefiles/Make.vars
include $(COMPILERSRCROOT)/Makefiles/Make.libs
include $(COMPILERSRCROOT)/Makefiles/Make.std

CXX = g++
CXXFLAGS = -g -O2 -Wall -W -ansi -pedantic -Woverloaded-virtual -Wcast-qual -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wconversion
BUILDDEPENDS = g++ -MM

MAKEFILE = Makefile

# Header files used within this library.
HDR = getclassstring.h getfilestring.h fakeObject.h serialisation.h

DISTFILES = Makefile.in configure configure.ac $(SRC) spar-rtl.h $(HDR)

#
#EXTRAFLAGS += -fno-implicit-templates
#CXXFLAGS += -D__GENERATE_SEQUENTIAL_CODE__
CXXFLAGS += -g
#CXXFLAGS += -pg

targets:	$(ALLLIBS)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."
	@$(ECHO) "tests            Test the library."

touch:
	touch $(SRC) 

# TODO: install all libs
install: $(ALLLIBS)
	$(INSTALL) -d ${exec_prefix}/lib
	$(INSTALL) -d ${prefix}/include
	$(INSTALL) -d $(INSTALL_INCLUDEDIR)
	$(INSTALL_DATA) $(LIB_S) ${exec_prefix}/lib/$(LIB_S)
	$(RANLIB) ${exec_prefix}/lib/$(LIB_S)
	$(INSTALL_DATA) spar-rtl.h $(INSTALL_INCLUDEDIR)/spar-rtl.h
	$(INSTALL_DATA) serialisation.h $(INSTALL_INCLUDEDIR)/serialisation.h

uninstall: 
	$(UNINSTALL_DATA) ${exec_prefix}/lib/$(LIB_S)
	$(UNINSTALL_DATA) $(INSTALL_INCLUDEDIR)/spar-rtl.h
	$(UNINSTALL_DATA) $(INSTALL_INCLUDEDIR)/serialisation.h

depend:	$(DEPENDFILE)

$(DEPENDFILE): $(SRC) $(HDR)

tests: 

check:

stats:
	wc -l *.cc *.h

genstats: $(GENSRC) $(GENHDR)
	echo "no generated files"

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist

-include $(DEPENDFILE)
