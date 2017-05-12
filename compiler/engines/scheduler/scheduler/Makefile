ECHO	   = echo
CC         = gcc
CXX        = g++
LDXX       = g++

MAKEFILE   = Makefile
DEPENDFILE = $(MAKEFILE).d
EXE	   = libsched.a
LIBS	   = 
CXXFLAGS   = -g -Wall
LDXXFLAGS  = -g -lm -lstdc++

# Various categories of source code files.
SRC = Scheduling.C ScheduleDAG.C \
      \
      CP_Node.C \
      LSAScheduling.C \
      LSFScheduling.C \
      HLFETNode.C SCFETNode.C SDFETNode.C \
      MD_Node.C DLSNode.C \
      LSA1Scheduling.C \
      HLFET1Node.C SCFET1Node.C SDFET1Node.C \
      MD1Node.C DLS1Node.C \
      LSA2Scheduling.C LSA2Node.C \
      HLFET2Node.C SCFET2Node.C SDFET2Node.C \
      MD2Node.C DLS2Node.C \
      MCPScheduling.C MCPNode.C \
      ETFScheduling.C ETFNode.C \
      DSA_Node.C \
      DLSScheduling.C \
      ERTScheduling.C ERTNode.C \
      DPSScheduling.C DPSNode.C \
      DPSFScheduling.C \
      FCP-f_Scheduling.C FCP-f_Node.C \
      FCP-p_Scheduling.C FCP-p_Node.C \
      FLB-f_Scheduling.C FLB-f_Node.C \
      FLB-p_Scheduling.C FLB-p_Node.C \
      FFF-f_Scheduling.C \
      FFF-p_Scheduling.C \
      FDLS-f_Scheduling.C FDLS-f_Node.C \
      FDLS-p_Scheduling.C FDLS-p_Node.C \
      \
      LC_Scheduling.C LC_Node.C \
      DSCScheduling.C DSCNode.C \
      WCMScheduling.C WCMNode.C \
      SARScheduling.C SARNode.C \
      LLBScheduling.C LLBNode.C  \
      GLBScheduling.C GLBNode.C GLBClusterItem.C
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
