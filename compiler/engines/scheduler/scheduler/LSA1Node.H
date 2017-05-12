/* $Id: LSA1Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __LSA1NODE_H
#define __LSA1NODE_H

#include "../data/PseudoHeapItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class LSA1Node *PLSA1Node;

/** Interface for List Scheduling Algorithms Task Nodes */
class LSA1Node :
  public NoDupDAGNode, public PseudoHeapItem
{
  public:
    LSA1Node (int nid, sched_flops flops)
        : NoDupDAGNode (nid, flops)
    {}
    virtual ~LSA1Node () {};

    virtual void ComputePrio (void) = NULL;
};


#endif
