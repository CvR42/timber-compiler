/* $Id: PrioDAGNode.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __PRIODAGNODE_H
#define __PRIODAGNODE_H

#include "../data/HeapItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class PrioDAGNode *PPrioDAGNode;

class PrioDAGNode :
  public NoDupDAGNode, public HeapItem
{
  public:
    PrioDAGNode (int nid, long flops);

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;


    sched_time ComputeOrderPrio (void);
    void SetOrderPrio (sched_time val);
    sched_time GetOrderPrio (void) const;


  protected:
    sched_time order_prio;     // The order_prio is used as priority in
                               // task ordering phase.
};

#endif

