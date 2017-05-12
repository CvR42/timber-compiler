/* $Id: SDFET2Node.H,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#ifndef __SDFET2NODE_H
#define __SDFET2NODE_H

#include "LSANode.H"


typedef class SDFET2Node *PSDFET2Node;

class SDFET2Node :
  public LSANode
{
  public:
    SDFET2Node (int nid, sched_flops flops);

    void ComputePrio (void);
    sched_time GetPrio (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
     /// Dynamic top level = the longest path from an entry task
     ///                     zeroing the communication between tasks
     ///                     scheduled on the same processor.
    sched_time dyn_t_level;

    sched_time ComputeDynTLevel (void);
};


#endif
