/* $Id: MD_Node.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __MD_NODE_H
#define __MD_NODE_H

#include "LSANode.H"


typedef class MD_Node *PMD_Node;

class MD_Node :
  public LSANode
{
  public:
    MD_Node (int nid, sched_flops flops);

    void ComputePrio (void);
    sched_time GetPrio (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
    sched_time AEST;        // Absolute Earliest Start Time
                            // (ASAP = As Soon As Possible binding of Wu)
    sched_time ALST;        // Absolute Lastest Start Time
                            // (ALAP = As Late As Possible binding of Wu)

    sched_time ComputeAEST (void);
    sched_time ComputeALST (void);
};


#endif
