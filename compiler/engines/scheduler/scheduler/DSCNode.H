/* $Id: DSCNode.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __DSCNODE_H
#define __DSCNODE_H

#include "../data/Sched.H"
#include "../dag/PrioDAGNode.H"


typedef class DSCNode *PDSCNode;

class DSCNode
  : public PrioDAGNode
{
  public:
    DSCNode (int nid, sched_flops flops);

    sched_time CalcBLevel (void);
    void ModifyTLevel (PDSCNode parent);
    sched_time GetTLevel (void) const;
    void SetTLevel (sched_time val);

    sched_time GetPrio (void) const;
    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;

    void IncNrExaminedParents (void);
    bool IsFree (void) const;
    bool IsNewPartialFree (void) const;

    void Print (void) const;
    void PrintVoid (void) const;


  private:
    sched_time b_level;    // The length of the longest path to the exit node.
    sched_time t_level;    // The length of the longest path to the entry node
                           // considering only examined nodes.

    unsigned int examined_parents;  // Number of parents examined:
                                    //  - initially set to 0
                                    //  - if greater than 0 => partially free
                                    //  - if equal to nr. of parents => free
};

#endif
