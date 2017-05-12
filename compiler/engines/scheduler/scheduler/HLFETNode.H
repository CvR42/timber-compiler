/* $Id: HLFETNode.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __HLFETNODE_H
#define __HLFETNODE_H

#include "LSANode.H"


typedef class HLFETNode *PHLFETNode;

class HLFETNode :
  public LSANode
{
  public:
    HLFETNode (int nid, sched_flops flops);

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
