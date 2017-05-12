/* $Id: FDLS-f_Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/arguments.h"
#include "../dag/DAGEdge.H"
#include "Scheduling.H"
#include "FDLS-f_Node.H"


FDLS_f_Node::FDLS_f_Node (int nid, long flops)
    : PrioDAGNode (nid, flops)
{
  b_level = NONE;
  emt = 0;
  lmt = 0;

  InitNrUnschedPred();
}

  
   /* Compute bottom levels by bottom-up topological traversing of the DAG */
sched_time
FDLS_f_Node::ComputeBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PFDLS_f_Node succ_node = (PFDLS_f_Node) out_edge->GetToNode();
    sched_time level = succ_node->ComputeBLevel() + 
                       out_edge->GetCommTime() + GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}


void
FDLS_f_Node::ComputeReadyVals (void)
{
  lmt = 0;
  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PFDLS_f_Node pred_node = (PFDLS_f_Node) in_edge->GetFromNode();
    sched_time t = pred_node->GetFinishTime() + in_edge->GetCommTime ();

    if (t > lmt)
    {
      lmt = t;
      proc_id = pred_node->GetProcID();
    }
  }

  emt = proc->GetProcReadyTime (proc_id);
  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PFDLS_f_Node pred_node = (PFDLS_f_Node) in_edge->GetFromNode();
    if (pred_node->proc_id == proc_id)
      continue;

    sched_time t = pred_node->GetFinishTime() + in_edge->GetCommTime ();

    if (t > emt)
      emt = t;
  }
}


sched_time
FDLS_f_Node::GetBLevel (void) const
{
  return b_level;
}

sched_time
FDLS_f_Node::GetEMT (void) const
{
  return emt;
}

sched_time
FDLS_f_Node::GetLMT (void) const
{
  return lmt;
}

sched_time
FDLS_f_Node::ComputeStartTimeOnProc (int pid) const
{
  sched_time ST;

  if (pid == proc_id)
    ST = emt;
  else
    ST = lmt;

  if (ST < proc->GetProcReadyTime (pid))
    ST = proc->GetProcReadyTime (pid);

  return ST;
}



sched_time
FDLS_f_Node::ComputePrio (int pid) const
{
  return b_level - ComputeStartTimeOnProc (pid);
}


bool
FDLS_f_Node::LessPrioThan (PHeapItem item) const
{
  PFDLS_f_Node node = (PFDLS_f_Node) item;

  if ((b_level - lmt) < (node->b_level - node->lmt))
    return true;

  return false;
}


bool
FDLS_f_Node::MorePrioThan (PHeapItem item) const
{
  PFDLS_f_Node node = (PFDLS_f_Node) item;

  if ((b_level - lmt) > (node->b_level - node->lmt))
    return true;

  return false;
}


void
FDLS_f_Node::Print (void) const
{
  printf (" %2d[%5.1f]", nid, b_level-lmt);
}


void
FDLS_f_Node::PrintVoid (void) const
{
  printf ("                       ");
}
