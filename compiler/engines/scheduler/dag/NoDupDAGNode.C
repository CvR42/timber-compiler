/* $Id: NoDupDAGNode.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../scheduler/Scheduling.H"
#include "../dag/DAGNode.H"
#include "../processors/ProcListItem.H"
#include "NoDupDAGNode.H"


NoDupDAGNode::NoDupDAGNode (int nid, long flops)
    : DAGNode (nid, flops)
{
  item_in_proc_list = 0;
  proc_id = NONE;
  seq_number_on_proc = NONE;
  FT = NONE;
}
  
  
void
NoDupDAGNode::SetProcID (int pid)
{
  proc_id = pid;
}

int
NoDupDAGNode::GetProcID (void) const
{
  return proc_id;
}


void
NoDupDAGNode::SetProcListItem (PProcListItem nl)
{
  item_in_proc_list = nl;
}

PProcListItem
NoDupDAGNode::GetProcListItem (void) const
{
  return item_in_proc_list;
}


void
NoDupDAGNode::SetSeqNrOnProc (int nr)
{
 seq_number_on_proc = nr;
}

int
NoDupDAGNode::GetSeqNrOnProc (void) const
{
  return seq_number_on_proc;
}


   /** Compute the start time of this node on a processor
       The start time is determined by 2 factors:
         - task dependencies (a task cannot start its execution until
            it has received all the data it needs)
         - processor utilization (a task cannot start its execution
            if the processor is busy)
       Return value:
         - the start time of this node on 'pid' */
sched_time
NoDupDAGNode::ComputeStartTimeOnProc (int pid)
{
  sched_time t, t_aux;

     // The time processor becoms idle is the earliest possible
     // start time of this node on 'pid'.
  if (pid != NONE)
    t = proc->GetProcReadyTime (pid);
  else
    t = 0;

     // Check if the delay due to the dependencies doesn't postpone
     // the start time.
  for (unsigned int pred_id = 0; pred_id < GetNrInEdges(); pred_id++)
  {
    PDAGEdge e = (PDAGEdge) GetInEdge (pred_id);
    PNoDupDAGNode pred = (PNoDupDAGNode) e->GetFromNode();

    t_aux = pred->GetFinishTime() + e->GetCommTime (pred->GetProcID(), pid);

    if (t_aux > t)
      t = t_aux;
  }

  return t;
}

sched_time
NoDupDAGNode::ComputeSTOnProc (int pid)
{
  sched_time t, t_aux;

     // The time processor becoms idle is the earliest possible
     // start time of this node on 'pid'.
  if (pid != NONE)
    t = proc->GetProcReadyTime (pid);
  else
    t = 0;

     // Check if the delay due to the dependencies doesn't postpone
     // the start time.
  for (unsigned int pred_id = 0; pred_id < GetNrInEdges(); pred_id++)
  {
    PDAGEdge e = (PDAGEdge) GetInEdge (pred_id);
    PNoDupDAGNode pred = (PNoDupDAGNode) e->GetFromNode();

    t_aux = pred->GetFT() + e->GetCommTime (pred->GetProcID(), pid);

    if (t_aux > t)
      t = t_aux;
  }

  return t;
}

sched_time
NoDupDAGNode::GetStartTime (void) const
{
  return item_in_proc_list->GetStartTime();
}


sched_time
NoDupDAGNode::GetFinishTime (void) const
{
  return item_in_proc_list->GetFinishTime();
}

sched_time
NoDupDAGNode::GetFT (void) const
{
  return FT;
}
void
NoDupDAGNode::SetFT (sched_time t)
{
  FT = t;
}


void
NoDupDAGNode::InitNrUnschedPred (void)
{
  unscheduled_pred = GetNrInEdges();
}

int
NoDupDAGNode::DecNrUnschedPred (void)
{
  unscheduled_pred--;
  return unscheduled_pred;
}

int
NoDupDAGNode::GetNrUnschedPred (void) const
{
  return unscheduled_pred;
}


void
NoDupDAGNode::SetOldStartTime (sched_time val)
{
  old_start_time = val;
}

sched_time
NoDupDAGNode::GetOldStartTime (void) const
{
  return old_start_time;
}

void
NoDupDAGNode::SetOldFinishTime (sched_time val)
{
  old_finish_time = val;
}

sched_time
NoDupDAGNode::GetOldFinishTime (void) const
{
  return old_finish_time;
}
