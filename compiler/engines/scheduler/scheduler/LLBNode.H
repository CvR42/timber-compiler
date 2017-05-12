/* $Id: LLBNode.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __LLBNODE_H
#define __LLBNODE_H

#include "../dag/CMrgDAGNode.H"
#include "../data/HeapItem.H"


typedef class LLBNode *PLLBNode;

class LLBNode
  : public CMrgDAGNode
{
  public:
    LLBNode (int nid, long flops);

    void SetTLevel (sched_time val);
    sched_time GetTLevel (void) const;

    sched_time ComputeBLevel (void);

    sched_time GetPrio (void) const;
    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;


  private:
    sched_time t_level;
    sched_time b_level;
};

#endif
