/* $Id: SCFET2Node.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __SCFET2NODE_H
#define __SCFET2NODE_H

#include "LSANode.H"


typedef class SCFET2Node *PSCFET2Node;

class SCFET2Node :
  public LSANode
{
  public:
    SCFET2Node (int nid, sched_flops flops);

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
