COMPILERSRCROOT = $(HOME)/timber/compiler
ifndef TMROOT
TMROOT = $(HOME)/local
endif

VNUSLIB = $(COMPILERSRCROOT)/backends/vnuslib
TMLIB = $(TMROOT)/lib
LOCALINC = -I$(TMROOT)/include -I$(VNUSLIB)

ECHO	= echo

# The magical stuff
-include $(COMPILERSRCROOT)/Makefiles/Make.rules
-include $(COMPILERSRCROOT)/Makefiles/Make.vars

WFLAGS   = -Wall -W -Wtraditional -Wpointer-arith -Wbad-function-cast -Wcast-align -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Woverloaded-virtual -Wstrict-prototypes -Wsynth # -ansi -pedantic
GNUWFLAGS = # -Waggregate-return # -Wconversion
ERRWFLAGS = # -Werror -pedantic-errors


CFLAGS	 = $(WFLAGS) $(ERRWFLAGS) $(GNUWFLAGS) $(LOCALINC) 
CXXFLAGS = -g $(WFLAGS) $(ERRWFLAGS) $(GNUWFLAGS) $(EXTRAFLAGS) $(LOCALINC) 

CC	 = gcc
LD	 = g++
MAKEFILE = Makefile
DEPENDFILE = $(MAKEFILE).d
EXE	 = tm2dag
LIBS	 = parser/libparser.a dag/libdag.a \
           scheduler/libsched.a processors/libproc.a data/libdata.a \
	   $(VNUSLIB)/libvnusctl.a $(TMROOT)/lib/libtmc.a
LDFLAGS	 = -g #-lstdc++
TM       = tm -I$(VNUSLIB) -I$(TMLIB) -I$(TMROOT)/include
VNUSDS	 = $(VNUSLIB)/vnus.ds

# Various categories of source code files.
TMSRC	   = $(VNUSDS) tmadmin.ds tmadmin.ct tmadmin.ht tmadmin.t \
             tm2dag.ct
SRC_CC     = Main.cc dag_interface.cc genC.cc io.cc error.cc global.cc \
             cardinals.cc tm_gen.cc evaluate.cc m_string.cc task.cc
SRC_C      = 

GENSRC  = tmadmin.cc tm2dag.cc
GENHDR  = tmadmin.h

OBJ	= $(SRC_CC:%.cc=%.o) $(GENSRC:%.cc=%.o)

# Sources we get dependency information from. The generated sources
# still need to be incorporated into this scheme.
DEPENDSRC = $(SRC_CC) $(GENSRC)

# Files to be deleted upon cleanup
JUNK    = $(GENSRC) $(GENHDR) $(OBJ) vnuserr \
	  makelog 


$(EXE): $(LIBS) $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) $(LIBS) -o $(EXE)

all: 	depend $(EXE)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all              Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."

gfiles:	$(GENHDR) $(GENSRC)

tmadmin.o: tmadmin.cc
tmadmin.cc: tmadmin.ds tmadmin.t tmadmin.ct extradefs.ht $(VNUSDS)
	$(TM) tmadmin.ds tmadmin.ct > $@
tmadmin.h: tmadmin.ds tmadmin.t tmadmin.ht extradefs.ht $(VNUSDS)
	$(TM) tmadmin.ds tmadmin.ht > $@
tm2dag.cc: tmadmin.t tmadmin.ds tm2dag.ct $(VNUSDS)
	$(TM) tmadmin.ds tm2dag.ct > tm2dag.cc

data/libdata.a: FORCE
	cd data; make
dag/libdag.a: FORCE
	cd dag; make
parser/libparser.a: FORCE
	cd parser; make
processors/libproc.a: FORCE
	cd processors; make
scheduler/libsched.a: FORCE
	cd scheduler; make


touch:
	touch $(LEXSRC) $(YACCSRC) $(SRC_CC) $(SRC_C) $(TMSRC)


clean:	
	rm -f $(JUNK)

empty:	clean
	rm -f $(EXE) $(DEPENDFILE)


depend:	gfiles createdepend 

createdepend: $(DEPENDFILE)
depend: $(DEPENDFILE)
	@$(ECHO) "Checking/Updating dependencies... "
	cd data; make depend
	cd dag; make depend
	cd parser; make depend
	cd processors; make depend
	cd scheduler; make depend
	@$(CXX) $(LOCALINC) $(CXXFLAGS) -MM $(DEPENDSRC) > $(DEPENDFILE)
	@$(ECHO) "done."

$(DEPENDFILE):
	touch $(DEPENDFILE)

%out:	%
	$< > $*.out 2>&1

.PRECIOUS:	%.g.cc %.g.v

FORCE:

-include $(DEPENDFILE)
