/* $Id: SCFET1Node.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __SCFET1NODE_H
#define __SCFET1NODE_H

#include "LSA1Node.H"


typedef class SCFET1Node *PSCFET1Node;

class SCFET1Node :
  public LSA1Node
{
  public:
    SCFET1Node (int nid, sched_flops flops);

    void ComputePrio (void);
    sched_time GetPrio (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
     /// Top level = the longest path from an entry task
    sched_time t_level;

    sched_time ComputeTLevel (void);
};


#endif
