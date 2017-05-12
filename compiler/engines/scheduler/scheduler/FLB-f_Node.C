/* $Id: FLB-f_Node.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/HeapItem.H"
#include "../dag/PrioDAGNode.H"
#include "../dag/DAGEdge.H"
#include "Scheduling.H"
#include "FLB-f_Node.H"


FLB_f_Node::FLB_f_Node (int nid, long flops)
    : PrioDAGNode (nid, flops)
{
  t_level = NONE;
  b_level = NONE;
  min_ST = NONE;

  InitNrUnschedPred();
}

  
   /* Calculate bottom levels by bottom-up topological traversing of the DAG */
sched_time
FLB_f_Node::ComputeBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PFLB_f_Node succ_node = (PFLB_f_Node) out_edge->GetToNode();
    sched_time level = succ_node->ComputeBLevel() + GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}


void
FLB_f_Node::ComputeReadyVals (void)
{
  t_level = 0;
  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PFLB_f_Node pred_node = (PFLB_f_Node) in_edge->GetFromNode();

    sched_time level = pred_node->GetFinishTime() +
                       in_edge->GetCommTime ();

    if (level > t_level)
    {
      t_level = level;
      proc_id = pred_node->GetProcID();
    }
  }

  min_ST = proc->GetProcReadyTime (proc_id);
  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PFLB_f_Node pred_node = (PFLB_f_Node) in_edge->GetFromNode();
    if (pred_node->proc_id == proc_id)
      continue;

    sched_time level = pred_node->GetFinishTime() +
                       in_edge->GetCommTime ();

    if (level > min_ST)
      min_ST = level;
  }
}


sched_time
FLB_f_Node::GetTLevel (void) const
{
  return t_level;
}

sched_time
FLB_f_Node::GetBLevel (void) const
{
  return b_level;
}

sched_time
FLB_f_Node::GetMinST (void) const
{
  return min_ST;
}

sched_time
FLB_f_Node::GetPrio (void) const
{
  if (proc_id != NONE)
    return min_ST + GetExecTime (proc_id);
  else
    switch (sched_alg)
    {
      case FLB_f:
      case FFF_f:
        return t_level;
      case FLB_f_b:
        return -b_level;
      default:
        fprintf (stderr, "Wrong algorithm: %d\n", sched_alg);
        exit(1);
        return 0;
    }
}

sched_time
FLB_f_Node::ComputeStartTimeOnProc (int pid) const
{
  sched_time ST;

  if (pid == proc_id)
    ST = min_ST;
  else
    ST = t_level;

  if (ST < proc->GetProcReadyTime (pid))
    ST = proc->GetProcReadyTime (pid);

  return ST;
}


bool
FLB_f_Node::LessPrioThan (PHeapItem item) const
{
  PFLB_f_Node node = (PFLB_f_Node) item;

  if (GetPrio() > node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (b_level < node->b_level)
      return true;

  return false;
}


bool
FLB_f_Node::MorePrioThan (PHeapItem item) const
{
  PFLB_f_Node node = (PFLB_f_Node) item;

  if (GetPrio() < node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (b_level > node->b_level)
      return true;

  return false;
}


void
FLB_f_Node::Print (void) const
{
  printf (" %2d[%5.1f,%5.1f/%5.1f]", nid, GetPrio(), b_level, t_level);
}


void
FLB_f_Node::PrintVoid (void) const
{
  printf ("                       ");
}

