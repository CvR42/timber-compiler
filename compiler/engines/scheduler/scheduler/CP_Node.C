/* $Id: CP_Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/arguments.h"
#include "../dag/DAGEdge.H"
#include "Scheduling.H"
#include "CP_Node.H"


CP_Node::CP_Node (int nid, long flops)
    : NoDupDAGNode (nid, flops)
{
  b_level = NONE;
  emt = 0;
  lmt = 0;
}

  
   /* Compute bottom levels by bottom-up topological traversing of the DAG */
sched_time
CP_Node::ComputeBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    CP_Node *succ_node = (CP_Node*) out_edge->GetToNode();
    sched_time level = succ_node->ComputeBLevel() + 
                       out_edge->GetCommTime() + GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}


void
CP_Node::ComputeReadyVals (void)
{
  lmt = -1;
  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    CP_Node *pred_node = (CP_Node*) in_edge->GetFromNode();
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
    CP_Node *pred_node = (CP_Node*) in_edge->GetFromNode();
    if (pred_node->proc_id == proc_id)
      continue;

    sched_time t = pred_node->GetFinishTime() + in_edge->GetCommTime ();

    if (t > emt)
      emt = t;
  }
}


sched_time
CP_Node::GetBLevel (void) const
{
  return b_level;
}

sched_time
CP_Node::GetEMT (void) const
{
  return emt;
}

sched_time
CP_Node::GetLMT (void) const
{
  return lmt;
}

sched_time
CP_Node::ComputeStartTimeOnProc (int pid) const
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
