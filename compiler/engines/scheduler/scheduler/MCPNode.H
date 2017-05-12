/* $Id: MCPNode.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __MCPNODE_H
#define __MCPNODE_H

#include "../dag/PrioDAGNode.H"


typedef class MCPNode *PMCPNode;

class MCPNode :
  public PrioDAGNode
{
  public:
    MCPNode (int nid, sched_flops flops);

    sched_time ComputeAEST (void);
    sched_time ComputeALST (void);

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
    sched_time AEST;        // Absolute Earliest Start Time
                            // (ASAP = As Soon As Possible binding of Wu)
    sched_time ALST;        // Absolute Lastest Start Time
                            // (ALAP = As Late As Possible binding of Wu)
};


#endif
