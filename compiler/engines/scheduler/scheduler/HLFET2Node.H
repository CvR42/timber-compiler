/* $Id: HLFET2Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __HLFET2NODE_H
#define __HLFET2NODE_H

#include "LSANode.H"


typedef class HLFET2Node *PHLFET2Node;

class HLFET2Node :
  public LSANode
{
  public:
    HLFET2Node (int nid, sched_flops flops);

    void ComputePrio (void);
    sched_time GetPrio (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
     /// Bottom level = the longest path to an exit task
    sched_time b_level;

    sched_time ComputeBLevel (void);
};


#endif
