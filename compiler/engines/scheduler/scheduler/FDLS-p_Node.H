/* $Id: FDLS-p_Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef _A_FDLS_P_NODE_H
#define _A_FDLS_P_NODE_H

#include "../data/PseudoHeapItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class FDLS_p_Node *PFDLS_p_Node;

class FDLS_p_Node
  : public NoDupDAGNode, public PseudoHeapItem
{
  public:
    FDLS_p_Node (int nid, long flops);

     /// Compute bottom levels by bottom-up topological traversing of the DAG
    sched_time ComputeBLevel (void);
     /// Compute LMT, EMT and EP
    void ComputeReadyVals (void);

    sched_time GetBLevel (void) const;
    sched_time GetEMT (void) const;
    sched_time GetLMT (void) const;
    int GetEP (void) const;

    sched_time ComputeStartTimeOnProc (int pid) const;
    sched_time ComputePrio (int pid) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
    sched_time b_level;
    sched_time emt;
    sched_time lmt;
};

#endif
