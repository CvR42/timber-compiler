/* $Id: CP_Node.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef _A_CP_NODE_H
#define _A_CP_NODE_H

#include "../dag/NoDupDAGNode.H"


class CP_Node
  : public NoDupDAGNode
{
  public:
    CP_Node (int nid, long flops);

     /// Compute bottom levels by bottom-up topological traversing of the DAG
    sched_time ComputeBLevel (void);
     /// Compute LMT, EMT and EP
    void ComputeReadyVals (void);

    sched_time GetBLevel (void) const;
    sched_time GetEMT (void) const;
    sched_time GetLMT (void) const;
    int GetEP (void) const;

    sched_time ComputeStartTimeOnProc (int pid) const;

  private:
    sched_time b_level;
    sched_time emt;
    sched_time lmt;
};

#endif
