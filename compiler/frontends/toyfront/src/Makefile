# $Author: leo $
# $Date: 1995/10/17 13:33:58 $
# $Revision: 1.1.1.1 $
# $Log: Makefile,v $
# Revision 1.1.1.1  1995/10/17 13:33:58  leo
# Initial frontend source tree.
#
# Revision 0.1  1995/05/08  11:24:35  joachim
# initial revision (derived from arnaud poelman)
#
# $Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/Makefile,v $
########


#
# Macro definitions
########

#
# where?
########

# location of cocktail toolbox
COCKTAIL = /usr/blob/lib/cocktail

# location of reuse library
LIB = $(COCKTAIL)/include
 
# root directory of integration project
PROJDIR = /home/godzilla/partool/integration/frontend/

# location of some test programs
TESTDIR = $(PROJDIR)/examples

#
# what?
########

# which shell
SHELL = /bin/sh

# which c-compiler do we use
CC = /usr/blob/bin/gcc

# these are our test programs
TESTPROG = HelloWorld

# these are the sources of the Booster front end
SRCS = Scope.c Scope.h booster.cg booster.pars booster.scan parserdriver.c \
         MagicNumbers.h ErrorStr.c ErrorStr.h Makefile scannerdriver.c

# these are the binaries to make a Booster scanner
SCANBINS = Errors.o scannerdriver.o boos_scanSource.o boos_scan.o ErrorStr.o

# these are the binaries to make a Booster parser
PARSBINS = parserdriver.o boos_pars.o boos_scan.o boos_scanSource.o\
           ErrorStr.o Errors.o Tree.o

# these are the binaries to make the Booster frontend
FRONTENDBINS = boos_pars.o boos_scan.o Errors.o\
               boos_scanSource.o ErrorStr.o Tree.o Scope.o\
               BuildScopes.o bfe.o
               
#
# how?
########

# flags for parser-generator
LALRFLAGS = -c -d -e -p -l

#flags for scanner-generator
REXFLAGS = -c -d -s  

#flags for tree/graph module generator
CGTREEFLAGS = -cdimwrgp4                #-cdimnfoRtbywrgpq=4

#flags for cg-lalr interface
CGLALRFLAGS = -cxzj 

#flags for cg-ag interface
CGAGFLAGS = -cDIOPMNC5

#flags for c-compiler
CFLAGS = -g  -I$(LIB) 

#flags for the linker
LDFLAGS = $(COCKTAIL)/libreuse.a -lm

#
# Dummy targets
########

all : scantest parstest bfe

new : clean all update

pack : 
	tar cfzv booster.tgz RCS

unpack :
	tar xzf booster.tgz

update :
	cp scantest  ../bin/scantest;
	cp parstest  ../bin/parstest

test : $(TESTPROG)

$(TESTPROG) :	
	parstest $(TESTDIR)/$@.boo 

clean :	
	rm -f *.o *.TS Statcg* scantest parstest bfe\
	boos_scan.c boos_scan.h\
	BuildScopes.c BuildScopes.h\
	boos_pars.c boos_pars.h\
	Tree.c Tree.h Errors.c Errors.h\
	yyDeclarations.w yyTree.w  \
	boos_pars.lalr boos_scan.rex boos_scan.rpp \
	boos_scanDrv.c boos_parsDrv.c \
	Semantics.c boos_scanSource.c boos_scanSource.h 


#
# Description entries
########


scantest : $(SCANBINS)
	$(CC) $(CFLAGS) -o scantest $(SCANBINS) $(LDFLAGS)

parstest : $(PARSBINS)
	$(CC) $(CFLAGS) -o parstest $(PARSBINS) $(LDFLAGS)
	
bfe : $(FRONTENDBINS)
	$(CC) $(CFLAGS) -o bfe $(FRONTENDBINS) $(LDFLAGS)

boos_parsDrv.c Tree.h Tree.c :	booster.cg
	cg $(CGTREEFLAGS) booster.cg > StatcgTree

Tree.TS : booster.cg
	echo SELECT AbstractSyntax Environment | cat - booster.cg | cg $(CGTREEFLAGS)

BuildScopes.h BuildScopes.c : booster.cg Tree.h
	  cg -cDI booster.cg > StatcgSem

boos_scan.h boos_scan.c : boos_scan.rex
	rex $(REXFLAGS) boos_scan.rex


Errors.h Errors.c boos_pars.h boos_pars.c : boos_pars.lalr 
	lalr $(LALRFLAGS) boos_pars.lalr

boos_scan.rex:	boos_scan.rpp booster.scan
	rpp boos_scan.rpp < booster.scan > boos_scan.rex

boos_scan.rpp boos_pars.lalr : booster.pars
	cg $(CGLALRFLAGS) booster.pars > StatcgPars

#
# Non-standard dependencies
########

boos_scan.o : boos_scan.h boos_scan.c ErrorStr.h

boos_scanSource.o : boos_scan.h boos_scan.c

ErrorStr.o : ErrorStr.h ErrorStr.c

scannerdriver.o: scannerdriver.c boos_scan.h 

parserdriver.o : parserdriver.c boos_pars.h Tree.h ErrorStr.h

boos_pars.o : boos_pars.h boos_pars.c Errors.h boos_scan.h Tree.h

Tree.o : Tree.h Tree.c ErrorStr.h

Semantics.o : Semantics.h Semantics.c Tree.h ErrorStr.h boos_pars.h

Errors.o : Errors.c Errors.h

BuildScopes.o : BuildScopes.h BuildScopes.c
