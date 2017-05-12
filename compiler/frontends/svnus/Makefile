
# The magical stuff
COMPILERSRCROOT=../..
include $(COMPILERSRCROOT)/Makefiles/Make.vars

#sidney: replaced by $TMROOT/lib and $TMROOT/include
#LOCALLIB = /usr/local/lib
#LOCALINC = /usr/local/include

#ECHO	= /usr/blob/bin/gnudir/echo
ECHO	= echo
YACC	 = bison -v -d -y

WFLAGS   = -Wall -W -Wtraditional -Wpointer-arith -Wbad-function-cast -Wcast-align -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Winline -Woverloaded-virtual -Wstrict-prototypes -Wsynth -ansi -pedantic

GNUWFLAGS = -Waggregate-return # -Wconversion

ERRWFLAGS = # -Werror -pedantic-errors

# CvR: Use LOGNEW (remove comment below) if you want to know where
# dangling allocations of tm datastructures occur.
USELOGNEW = -DLOGNEW

CFLAGS	 = $(USELOGNEW) -g $(WFLAGS) $(ERRWFLAGS) $(GNUWFLAGS) -D__USE_FIXED_PROTOTYPES__ -I$(TMROOT)/include # -a
CXXFLAGS = $(USELOGNEW) -O2 -g $(WFLAGS) $(ERRWFLAGS) -I$(TMROOT)/include

CC	 = g++
LD	 = $(CC)
MAKEFILE = Makefile
DEPENDFILE = $(MAKEFILE).d
EXE	 = svnus
LIBS	 = $(TMROOT)/libtmc.a
LDFLAGS	 = -g
TM       = tm

OBJ	= parser.o lex.o symbol_table.o main.o error.o global.o tmadmin.o \
	  prettyprint.o check.o extract.o collect.o mangle.o typederive.o \
	  rewrite.o service.o

# Generated sources and headers
GENSRC  = parser.c tmadmin.c mangle.c extract.c check.c rewrite.c
GENHDR  = tokens.h tmadmin.h


# Sources we get dependency information from.
DEPENDSRC = $(GENSRC) symbol_table.c main.c error.c global.c \
	    check.c prettyprint.c mangle.c collect.c extract.c typederive.c \
	    rewrite.c

# Files to be deleted upon cleanup
JUNK    = $(GENSRC) $(GENHDR) $(OBJ) y.output lex.back* vnuserr \
	  makelog runout testpar-t testout testseq-s testpar-t.g.* \
	  testseq-s.g.* test-*.err testseq-s.err testpar-d.err

all: 	depend $(EXE)

$(EXE): $(OBJ) $(LIBS)
	$(LD) $(LDFLAGS) $(OBJ) $(LIBS) -o $(EXE)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."
	@$(ECHO) "tests            Do some test runs of the compiler."
	@$(ECHO) "runs             Do some test runs of the compiler output."
	@$(ECHO) "Sequential code test:"
	@$(ECHO) "test-s           Run the compiler, and examine output."
	@$(ECHO) "run-s            Compile and run, and examine output of that run."
	@$(ECHO) "Threaded code test:"
	@$(ECHO) "test-t           Run the compiler, and examine output."
	@$(ECHO) "run-t            Compile and run, and examine output of that run."

gfiles:	$(GENHDR) $(GENSRC)

parser.c tokens.h: parser.y
	$(YACC) $<
	mv y.tab.c parser.c
	mv y.tab.h tokens.h

rewrite.c: svnus.ds tmadmin.ds rewrite.ct
	$(TM) tmadmin.ds rewrite.ct > rewrite.c

check.c: svnus.ds tmadmin.ds check.ct
	$(TM) tmadmin.ds check.ct > check.c

extract.c: svnus.ds tmadmin.ds extract.ct
	$(TM) tmadmin.ds extract.ct > extract.c

mangle.c: svnus.ds tmadmin.ds mangle.ct
	$(TM) tmadmin.ds mangle.ct > mangle.c

tmadmin.c: svnus.ds tmadmin.ds tmadmin.t tmadmin.ct
	$(TM) tmadmin.ds tmadmin.ct > tmadmin.c

tmadmin.h: svnus.ds tmadmin.ds tmadmin.t tmadmin.ht
	$(TM) tmadmin.ds tmadmin.ht > tmadmin.h

test-s: testseq-s.g.v testseq-s.err good-s.v good-s.err
	diff testseq-s.g.v good-s.v
	diff testseq-s.err good-s.err

testseq-s.g.v testseq-s.err: $(EXE) testseq-s.sv
	$(EXE) testseq-s.sv -o testseq-s.g.v -e testseq-s.err

run-s: testseq-s
	testseq-s
	diff runout good-s.out

test-t: testpar-d.g.v testpar-d.err good-d.v good-d.err
	diff testpar-d.g.v good-d.v
	diff testpar-d.err good-d.err

testpar-d.g.v testpar-d.err: $(EXE) testpar-d.sv
	$(EXE) testpar-d.sv -o testpar-d.g.v -e testpar-d.err

run-t: testpar-t
	testpar-t
	diff runout good-t.out

tests: test-s test-t

runs: run-s run-t

clean:	
	rm -f $(JUNK)

empty:	clean
	rm -f $(EXE) $(DEPENDFILE)


depend:	gfiles createdepend 

createdepend: $(DEPENDFILE)
	@$(ECHO) -n "Checking/Updating dependencies... "
	@$(CC) -MM $(DEPENDSRC) > $(DEPENDFILE)
	@$(ECHO) "done."

$(DEPENDFILE):
	touch $(DEPENDFILE)

#%out:	%
#	$< > $*.out 2>&1
#
#%test:	%out
#	diff $*.out $*.good.out

.PRECIOUS:	%.g.cc

-include $(DEPENDFILE)
