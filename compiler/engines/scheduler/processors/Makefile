ECHO	   = echo
CC         = gcc
CXX        = g++
LDXX       = g++

MAKEFILE   = Makefile
DEPENDFILE = $(MAKEFILE).d
EXE	   = libproc.a
CXXFLAGS   = -g -Wall


# Various categories of source code files.
SRC = Processors.C ProcList.C ProcListItem.C Simulator.C
OBJ = $(SRC:%.C=%.o)

# Files to be deleted upon cleanup
JUNK       = $(OBJ)


all: 	$(EXE)

$(EXE): $(OBJ) $(LIBS)
	ar -rs $(EXE) $(OBJ)

help:
	@$(ECHO) " Possible make targets:"
	@$(ECHO) "all(default)     Create local running programs."
	@$(ECHO) "clean            Delete all temporary files."
	@$(ECHO) "empty            Delete all temporary files and executables."
	@$(ECHO) "depend           Update dependencies in $(DEPENDFILE)."

clean:	
	rm -f $(JUNK)

empty:	clean
	rm -f $(EXE)

depend: 
	@$(CXX) -MM $(SRC) | sed -e "s/\(.*\.o: \)\(.*\/\)\(.*\.C\)/\2\1\2\3/" > $(DEPENDFILE)

%.o:    %.C
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPENDFILE)
