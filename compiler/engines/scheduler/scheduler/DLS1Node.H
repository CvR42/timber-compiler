/* $Id: DLS1Node.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __DLS1NODE_H
#define __DLS1NODE_H

#include "LSA1Node.H"


typedef class DLS1Node *PDLS1Node;

class DLS1Node :
  public LSA1Node
{
  public:
    DLS1Node (int nid, sched_flops flops);

    void ComputePrio (void);
    sched_time GetPrio (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
     /// Bottom level = the longest path to an exit task
    sched_time b_level;
     /// Dynamic top level = the longest path from an entry task
     ///                     zeroing the communication between tasks
     ///                     scheduled on the same processor.
    sched_time dyn_t_level;

    sched_time ComputeBLevel (void);
    sched_time ComputeDynTLevel (void);
};


#endif
