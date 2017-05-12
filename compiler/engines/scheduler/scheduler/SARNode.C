/* $Id: SARNode.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <assert.h>

#include "../data/Sched.H"
#include "../dag/DAGEdge.H"
#include "../dag/CMrgDAGNode.H"
#include "SARNode.H"


SARNode::SARNode (int nid, int flops)
    : CMrgDAGNode (nid, flops)
{
}

   /**
      Compute the start time of the task, as if each cluster would
      represent a processor.
   */
sched_time
SARNode::ComputeLastMesgArrivalTime (int old_cluster, int dest_pid)
{
  sched_time EST = 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge edge = (PDAGEdge) GetInEdge (i);
    PSARNode pred = (PSARNode) edge->GetFromNode();

    assert (pred->GetFinishTime() != NONE);
    sched_time tmp = pred->GetFinishTime();

    if (!(pred->virtual_processor == virtual_processor))
      if (!((pred->proc_id == dest_pid) &&
            (proc_id == NONE) && (virtual_processor == old_cluster)))
        if (!((proc_id == dest_pid) &&
              (pred->proc_id == NONE) && (pred->virtual_processor == old_cluster)))
          tmp += edge->GetCommTime (pred->proc_id, proc_id);

    if (tmp > EST)
      EST = tmp;
  }

  return EST;
}


void
SARNode::SetStartTime (sched_time time)
{
  start_time = time;
}

sched_time
SARNode::GetStartTime (void) const
{
  return start_time;
}

sched_time
SARNode::GetFinishTime (void) const
{
  return start_time + GetExecTime (proc_id);
}
