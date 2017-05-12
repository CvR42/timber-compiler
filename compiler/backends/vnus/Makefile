#Makefile for Vnus backend.



COMPILERSRCROOT=../..
TMROOT=/Users/reeuwijk/lab

prefix=/Users/reeuwijk/lab
exec_prefix=${prefix}
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
UNINSTALL_PROGRAM = rm -rf
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

VNUSLIB=../vnuslib

# The magical stuff
include $(COMPILERSRCROOT)/Makefiles/Make.vars

#ECHO	= /usr/blob/bin/gnudir/echo
ECHO	= echo
YACC	 = bison -y -v -d
DRIVER	 = $(COMPILERSRCROOT)/driver/spar
CC	= g++
#EFENCE = -lefence
BUILDDEPENDS = g++ -MM

#CHECKCAST = -DTMC_CHECKED_CAST

GNUWFLAGS = # -Waggregate-return -Wconversion

ERRWFLAGS = # -Werror -pedantic-errors

EXTRAFLAGS += $(CHECKCAST) -g -I$(VNUSLIB) -I$(TMROOT)/include -O2
#EXTRAFLAGS += -DLOGNEW

LINTFLAGS = -i$(TMROOT)/include -i../vnuslib


MAKEFILE = Makefile
DEPENDFILE = $(MAKEFILE).d
EXE	 = vnus
LD	= $(CXX)
LIBS	 = $(VNUSLIB)/libvnusctl.a $(TMROOT)/lib/libtmc.a #-lefence
LDFLAGS	 = -g $(EFENCE)
TM       = tm -I$(VNUSLIB)
VNUSDS   = $(VNUSLIB)/vnus.ds

# Various categories of source code files.
TMSRC	= tmadmin.ds tmadmin.ct tmadmin.ht tmadmin.t rewrite.ct check.ct \
  collectassigned.ct ispure.ct
YACCSRC = parser.y
SRC	= symbol_table.c main.c error.c global.c generate.c collect.c \
          pragma.c service.c type.c lex.c typename.c analysis.c \
	  symbol_support.c

#GENSRC  = parser.c lexer.c tmadmin.c
GENSRC  = parser.c tmadmin.c rewrite.c check.c collectassigned.c ispure.c
GENHDR  = tokens.h tmadmin.h

OBJ	= $(SRC:%.c=%.o) $(GENSRC:%.c=%.o)

# Non-generated header files.
HDRS = check.h collectassigned.h collect.h defs.h error.h ispure.h \
    generate.h global.h lex.h parser.h pragma.h service.h symbol_support.h \
    symbol_table.h type.h typename.h rewrite.h analysis.h

TESTFILES = testseq-s.v testpar-d.v good-s.err good-s.g.cc good-d.g.cc \
    good-d.err good-s.out

DISTFILES = Makefile.in configure configure.ac $(SRC) $(YACCSRC) $(TMSRC) $(HDRS) $(TESTFILES)

# Sources we get dependency information from. The generated sources
# still need to be incorporated into this scheme.
DEPENDSRC = $(SRC) $(GENSRC)
DEPENDHDR = $(GENHDR) $(HDRS)

# Files to be deleted upon cleanup
JUNK    = $(GENSRC) $(GENHDR) $(OBJ) y.output lex.back* vnuserr \
	  makelog run-s.out runout testseq-s testpar-t testpar-d testout \
	  testseq-s.g.* testpar-t.g.* testpar-d.g.* testseq-s.err testpar-t.err testpar-d.err \
	  tmp

all: 	depend $(EXE)

$(EXE): $(OBJ) $(LIBS)
	$(LD) $(LDFLAGS) $(OBJ) $(LIBS) -o $(EXE)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "test             Run the compiler, and examine output."
	@$(ECHO) "testrun          Compile and run compiler output, and examine output of that run."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."
	@$(ECHO) "tests            Do some test runs of the compiler."
	@$(ECHO) "check            Do some test runs of the compiler."
	@$(ECHO) "runs             Do some test runs of the compiler output."
	@$(ECHO) "Sequential code test:"
	@$(ECHO) "test-s           Run the compiler, and examine output."
	@$(ECHO) "run-s            Compile and run, and examine output of that run."
	@$(ECHO) "Threaded code test:"
	@$(ECHO) "test-t           Run the compiler, and examine output."
	@$(ECHO) "run-t            Compile and run, and examine output of that run."
	@$(ECHO) "Distributed code test:"
	@$(ECHO) "test-d           Run the compiler, and examine output."
	@$(ECHO) "run-d            Compile and run, and examine output of that run."

# CvR: The action below is just to be compatible with the 'check' target';
tests: test-s test-d
	@$(ECHO) "Tests completed"

# CvR: The action below is just to tell make that this is not a dependency
# for the executable 'check' to be built from 'check.c'.
check: test-s test-d
	@$(ECHO) "Tests completed"

install: $(EXE)
	$(INSTALL) -d ${exec_prefix}/bin
	$(INSTALL_PROGRAM) $(EXE) ${exec_prefix}/bin/$(EXE)

uninstall:
	$(UNINSTALL_PROGRAM) ${exec_prefix}/bin/$(EXE)

gfiles:	$(GENHDR) $(GENSRC)

parser.c tokens.h: parser.y
	$(YACC) $<
	mv y.tab.c parser.c
	mv y.tab.h tokens.h

collectassigned.c: tmadmin.ds collectassigned.ct $(VNUSDS)
	$(TM) tmadmin.ds collectassigned.ct > collectassigned.c

tmadmin.c: tmadmin.ds tmadmin.t tmadmin.ct $(VNUSDS)
	$(TM) tmadmin.ds tmadmin.ct > tmadmin.c

tmadmin.h: tmadmin.ds tmadmin.t tmadmin.ht $(VNUSDS)
	$(TM) tmadmin.ds tmadmin.ht > tmadmin.h

t.c: t.ct tmadmin.ds $(VNUSDS)
	$(TM) tmadmin.ds t.ct > t.c

rewrite.c: rewrite.ct tmadmin.ds $(VNUSDS)
	$(TM) tmadmin.ds rewrite.ct > rewrite.c

check.c: check.ct tmadmin.ds $(VNUSDS)
	$(TM) tmadmin.ds check.ct > check.c

ispure.c: ispure.ct tmadmin.ds $(VNUSDS)
	$(TM) tmadmin.ds ispure.ct > ispure.c

touch:
	touch $(YACCSRC) $(SRC) $(TMSRC)

test-s: testseq-s.g.cc testseq-s.err
	diff testseq-s.err good-s.err
	diff testseq-s.g.cc good-s.g.cc

# This roundabout way is used to test Tm input and output
testseq-s.g.cc: $(EXE) testseq-s.v
	./$(EXE) -t testseq-s.v -o tmp 2> testseq-s.err
	./$(EXE) -r tmp -o testseq-s.g.cc -e tmperr -I ctype.h
	cat tmperr >> testseq-s.err
	rm -f tmperr tmp

good-s.out:
	touch good-s.out

run-s: testseq-s
	./testseq-s
	diff runout good-s.out

test-t: testpar-t.g.cc testpar-t.err
	diff testpar-t.g.cc good-t.g.cc
	diff testpar-t.err good-t.err

testpar-t testpar-t.err: $(EXE) testpar-d.v
	$(DRIVER) --parallel-vnus -a5 testpar-d.v -o testpar-t 2> testpar-t.err

run-t: testpar-t
	./testpar-t
	diff runout good-t.out

test-d: testpar-d.g.cc testpar-d.err
	diff testpar-d.g.cc good-d.g.cc
	diff testpar-d.err good-d.err

testseq-s testseq-s.err: $(EXE) testseq-s.v
	$(DRIVER) testseq-s.v -o testseq-s 2> testseq-s.err

testpar-d testpar-d.err: $(EXE) testpar-d.v
	$(DRIVER) --parallel-vnus -a5 testpar-d.v -o testpar-d 2> testpar-d.err

testpar-d.g.cc: $(EXE) testpar-d.v
	./$(EXE) -p testpar-d.v -o testpar-d.g.cc -e testpar-d.err

run-d: testpar-d
	testpar-d
	diff runout good-d.out

runs: run-s

depend:	$(DEPENDFILE)

$(DEPENDFILE): $(DEPENDSRC) $(DEPENDHDR)
	$(BUILDDEPENDS) $(CFLAGS) $(DEPENDSRC) > $(DEPENDFILE)

%out:	%
	$< > $*.out 2>&1

%test:	%out
	diff $*.out $*.good.out

stats: empty
	wc -l *.c *.h *.y *.ct *.ht

genstats: $(GENSRC) $(GENHDR)
	wc -l $(GENSRC) $(GENHDR)

lint: $(SRC) $(GENSRC) $(GENHDR) $(HDRS) co-gnu.lnt fnprop.lnt suppress.lnt
	lint co-gnu fnprop suppress $(LINTFLAGS) $(SRC) $(GENSRC) > lint

.PRECIOUS:	%.g.cc %.g.v

SUBDIRS = testsuite doc

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.std

-include $(DEPENDFILE)

testpar-d: testpar-d.o
lex.o: tokens.h
$(EXE): $(VNUSLIB)/libvnusctl.a
