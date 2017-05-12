/* $Id: SARNode.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __SARNODE_H
#define __SARNODE_H

#include "../dag/CMrgDAGNode.H"


typedef class SARNode *PSARNode;

class SARNode
  : public CMrgDAGNode
{
  public:
    SARNode (int nid, int flops);

     /** Compute the start time of the task, as if each cluster would
         represent a processor. */
    sched_time ComputeLastMesgArrivalTime (int old_cluster, int dest_pid);

    void SetStartTime (sched_time time);
    sched_time GetStartTime (void) const;
    sched_time GetFinishTime (void) const;

  private:
    sched_time start_time;
};

#endif
