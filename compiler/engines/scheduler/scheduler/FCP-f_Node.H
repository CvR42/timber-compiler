/* $Id: FCP-f_Node.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __FCP_f_NODE_H
#define __FCP_f_NODE_H

#include "../dag/PrioDAGNode.H"


typedef class FCP_f_Node *PFCP_f_Node;

class FCP_f_Node
  : public PrioDAGNode
{
  public:
    FCP_f_Node (int nid, long flops);

    sched_time ComputeBLevel (void);

    sched_time GetPrio (void) const;
    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void Print (void) const;
    void PrintVoid (void) const;

    bool operator< (const PFCP_f_Node &b) const 
    {
      return LessPrioThan (b);
    }

  private:
    sched_time b_level;
};

#endif
