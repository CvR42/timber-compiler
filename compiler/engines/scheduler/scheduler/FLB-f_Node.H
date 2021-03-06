/* $Id: FLB-f_Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __FLB_f_NODE_H
#define __FLB_f_NODE_H

#include "../dag/PrioDAGNode.H"


typedef class FLB_f_Node *PFLB_f_Node;

class FLB_f_Node
  : public PrioDAGNode
{
  public:
    FLB_f_Node (int nid, long flops);

   /* Calculate bottom levels by bottom-up topological traversing of the DAG */
    sched_time ComputeBLevel (void);
    void ComputeReadyVals (void);
    sched_time GetTLevel (void) const;
    sched_time GetBLevel (void) const;
    sched_time GetMinST (void) const;
    sched_time GetPrio (void) const;
    sched_time ComputeStartTimeOnProc (int pid) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;


  private:
    sched_time t_level;
    sched_time b_level;
    sched_time min_ST;
};

#endif
