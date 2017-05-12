/* $Id: DPSNode.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __DPSNODE_H
#define __DPSNODE_H

#include "../data/ListItem.H"
#include "../dag/PrioDAGNode.H"


typedef class DPSNode *PDPSNode;

class DPSNode :
  public PrioDAGNode, public ListItem
{
  public:
    DPSNode (int nid, sched_flops flops);

    sched_time ComputeTopLevel (void);

    bool InTaskList (void) const;
    void SetInTaskList (void);

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
    sched_time top_level;   // Top Level
    bool in_task_list;
};


#endif
