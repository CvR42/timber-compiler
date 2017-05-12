/* $Id: HLFET1Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __HLFET1NODE_H
#define __HLFET1NODE_H

#include "LSA1Node.H"


typedef class HLFET1Node *PHLFET1Node;

class HLFET1Node :
  public LSA1Node
{
  public:
    HLFET1Node (int nid, sched_flops flops);

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
