/* $Id: LC_Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __LC_NODE_H
#define __LC_NODE_H

#include "../data/Sched.H"
#include "../dag/PrioDAGNode.H"


typedef class LC_Node *PLC_Node;

class LC_Node
  : public PrioDAGNode
{
  public:
    LC_Node (int nid, sched_flops flops);

   /** Node level is the length of the longest path from the current
       node to an exit node including communication.
       Depending on the parameter, the already mapped tasks
       are considered or not. */
    sched_time ComputeBLevel (bool consider_mapped_tasks);
    void ResetBLevel (void);
    sched_time GetBLevel (void) const;

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;
    void Print (void) const;
    void PrintVoid (void) const;
  
  
  private:
     /** The longest path to the exit task
          - used as a priority criterion in heap */
    sched_time b_level;
};

#endif
