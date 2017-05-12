/* $Id: SDFET1Node.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __SDFET1NODE_H
#define __SDFET1NODE_H

#include "LSA1Node.H"


typedef class SDFET1Node *PSDFET1Node;

class SDFET1Node :
  public LSA1Node
{
  public:
    SDFET1Node (int nid, sched_flops flops);

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
