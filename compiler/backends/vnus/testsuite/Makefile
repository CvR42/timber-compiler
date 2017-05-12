MAKEFILE = Makefile

COMPILERSRCROOT=../../..

SCRIPTS = runall runerr rungen runok.in runrte.in

TESTFILES = $(wildcard *.v *.out *.err *.flags ok*.status err*.status vfy*.status rte*.status gen*.c)

CONFIGUREFILES = configure configure.ac

DISTFILES = $(MAKEFILE).in $(SCRIPTS) $(TESTFILES) $(CONFIGUREFILES)

ERRTESTS = $(wildcard err*.v)
OKTESTS = $(wildcard ok*.v)
GENTESTS = $(wildcard gen*.v)
RTETESTS = $(wildcard rte*.v)

TESTS = $(ERRTESTS) $(OKTESTS) $(GENTESTS) $(RTETESTS)
DONEFILES = $(TESTS:.v=.done)

JUNK =
PRECIOUSJUNK = runok runrte $(DONEFILES)

# The files that are tested: if they change, the testsuite must be
# run again.
REFFILES = ../vnus

all:

help:
	@echo " Possible make targets:"
	@echo "all              Create local running programs."
	@echo "clean            Delete all temporary files."
	@echo "empty            Delete all temporary files and executables."
	@echo "check            Run all tests."
	@echo "help             This help text."

err%.done: err%.v runerr
	sh runerr $<
	touch $@

gen%.done: gen%.v rungen
	sh rungen $<
	touch $@

ok%.done: ok%.v runok
	sh runok $<
	touch $@

rte%.done: rte%.v runrte
	sh runrte $<
	touch $@

test: $(DONEFILES)

tests: $(DONEFILES)

check: $(DONEFILES)

include $(COMPILERSRCROOT)/Makefiles/Make.configure
include $(COMPILERSRCROOT)/Makefiles/Make.dist
include $(COMPILERSRCROOT)/Makefiles/Make.std

$(DONEFILES): $(REFFILES)
