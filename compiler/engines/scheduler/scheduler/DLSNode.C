/* $Id: DLSNode.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "DLSNode.H"


DLSNode::DLSNode (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  b_level = NONE;
  dyn_t_level = NONE;
}


void
DLSNode::ComputePrio (void)
{
  ComputeBLevel();
  ComputeDynTLevel();
}

sched_time
DLSNode::GetPrio (void) const
{
  return b_level - dyn_t_level;
}


sched_time
DLSNode::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PDLSNode succ = (PDLSNode) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


sched_time
DLSNode::ComputeDynTLevel ()
{
  if (dyn_t_level != NONE)
    return dyn_t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PDLSNode pred = (PDLSNode) in_edge->GetFromNode();

    sched_time t_lev = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_lev > dyn_t_level)
      dyn_t_level = t_lev;
  }

  return dyn_t_level;
}


bool
DLSNode::LessPrioThan (PHeapItem item) const
{
  PDLSNode the_other = (PDLSNode) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
DLSNode::MorePrioThan (PHeapItem item) const
{
  PDLSNode the_other = (PDLSNode) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
DLSNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
DLSNode::PrintVoid (void) const
{
  printf ("      ");
}
