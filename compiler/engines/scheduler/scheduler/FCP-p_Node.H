/* $Id: FCP-p_Node.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __FCP_p_NODE_H
#define __FCP_p_NODE_H

#include "../data/HeapItem.H"
#include "../data/PseudoHeapItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class FCP_p_Node *PFCP_p_Node;

class FCP_p_Node
  : public NoDupDAGNode, public PseudoHeapItem
{
  public:
    FCP_p_Node (int nid, long flops);

    sched_time ComputeBLevel (void);

    sched_time GetPrio (void) const;
    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;


  private:
    sched_time b_level;
};

#endif
