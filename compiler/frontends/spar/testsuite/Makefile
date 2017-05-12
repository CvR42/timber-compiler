COMPILERSRCROOT=../../..
VNUSEXE=$(COMPILERSRCROOT)/backends/vnus/vnus

ERRTESTSSPAR = $(wildcard err*.spar)
ERRTESTSJAVA = $(wildcard err*.java)

GENTESTSSPAR = $(wildcard gen*.spar)
GENTESTSJAVA = $(wildcard gen*.java)

VFYTESTSSPAR = $(wildcard vfy*.spar)
VFYTESTSJAVA = $(wildcard vfy*.java)

OKTESTSSPAR = $(wildcard ok*.spar)
OKTESTSJAVA = $(wildcard ok*.java)

RTETESTSSPAR = $(wildcard rte*.spar)
RTETESTSJAVA = $(wildcard rte*.java)

TESTSSPAR = $(ERRTESTSSPAR) $(OKTESTSSPAR) $(VFYTESTSSPAR) $(GENTESTSSPAR) $(RTETESTSSPAR) 
TESTSJAVA = $(ERRTESTSJAVA) $(OKTESTSJAVA) $(VFYTESTSJAVA) $(GENTESTSJAVA) $(RTETESTSJAVA)

# A representative output file for each class of test
DONEFILES = $(TESTSJAVA:%.java=%.done) $(TESTSSPAR:%.spar=%.done)

# For make check we omit the 'gen' tests.
CHECKDONEFILES = \
    $(ERRTESTSJAVA:%.java=%.done) \
    $(OKTESTSJAVA:%.java=%.done) \
    $(VFYTESTSJAVA:%.java=%.done) \
    $(RTETESTSJAVA:%.java=%.done) \
    $(ERRTESTSSPAR:%.spar=%.done) \
    $(OKTESTSSPAR:%.spar=%.done) \
    $(VFYTESTSSPAR:%.spar=%.done) \
    $(RTETESTSSPAR:%.spar=%.done)

JUNK = $(DONEFILES) *.testout *.testerr *.tv *.exe dist.lst

PRECIOUSJUNK = *.gprof $(CHECKDONEFILES)

MAKEFILE = Makefile

SCRIPTS = runok runerr rungen runrte runall

TESTFILES = $(wildcard *.flags *.spar *.argv *.err *.v *.out err*.status gen*.status vfy*.status ok*.status rte*.status) okjclecho.in

DISTFILES = $(MAKEFILE) $(TESTFILES) $(SCRIPTS)

all:

# The files that are tested: if they change, the testsuite must
# be run again.
REFFILES = ../../../driver/spar ../sparfront

# Rules to make the .done file of each catagory: run the 'run' script
# of the class, and after that touch the .done file
ok%.done: ok%.spar runok
	./runok $<
	touch $@

err%.done: err%.spar runerr
	./runerr $<
	touch $@

gen%.done: gen%.spar rungen
	./rungen $<
	touch $@

vfy%.done: vfy%.spar runvfy
	./runvfy $<
	touch $@

rte%.done: rte%.spar runrte
	./runrte $<
	touch $@

ok%.done: ok%.java runok
	./runok $<
	touch $@

err%.done: err%.java runerr
	./runerr $<
	touch $@

gen%.done: gen%.java rungen
	./rungen $<
	touch $@

vfy%.done: vfy%.java runvfy
	./runvfy $<
	touch $@

rte%.done: rte%.java runrte
	./runrte $<
	touch $@

check: $(CHECKDONEFILES)

tests: $(DONEFILES)

clean:
	rm -f $(JUNK)

empty:
	rm -f $(JUNK) $(PRECIOUSJUNK)

SUBDIRS = subpack

include $(COMPILERSRCROOT)/Makefiles/Make.dist

$(DONEFILES): $(REFFILES)
$(RTETESTS:.spar=.done) $(OKTESTS:.spar=.done): $(VNUSEXE)
