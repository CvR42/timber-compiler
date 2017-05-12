/* $Id: DLS2Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "DLS2Node.H"


DLS2Node::DLS2Node (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  b_level = NONE;
  dyn_t_level = NONE;
}


void
DLS2Node::ComputePrio (void)
{
  ComputeBLevel();
  ComputeDynTLevel();
}

sched_time
DLS2Node::GetPrio (void) const
{
  return b_level - dyn_t_level;
}


sched_time
DLS2Node::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PDLS2Node succ = (PDLS2Node) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


sched_time
DLS2Node::ComputeDynTLevel ()
{
  if (dyn_t_level != NONE)
    return dyn_t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PDLS2Node pred = (PDLS2Node) in_edge->GetFromNode();

    sched_time t_lev = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_lev > dyn_t_level)
      dyn_t_level = t_lev;
  }

  return dyn_t_level;
}


bool
DLS2Node::LessPrioThan (PHeapItem item) const
{
  PDLS2Node the_other = (PDLS2Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
DLS2Node::MorePrioThan (PHeapItem item) const
{
  PDLS2Node the_other = (PDLS2Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
DLS2Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
DLS2Node::PrintVoid (void) const
{
  printf ("      ");
}
