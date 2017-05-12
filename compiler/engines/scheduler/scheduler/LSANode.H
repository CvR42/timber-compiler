/* $Id: LSANode.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSANODE_H
#define __LSANODE_H

#include "../dag/PrioDAGNode.H"


typedef class LSANode *PLSANode;

/** Interface for List Scheduling Algorithms Task Nodes */
class LSANode :
  public PrioDAGNode
{
  public:
    LSANode (int nid, sched_flops flops)
        : PrioDAGNode (nid, flops)
    {}

    virtual void ComputePrio (void) = NULL;
};


#endif
