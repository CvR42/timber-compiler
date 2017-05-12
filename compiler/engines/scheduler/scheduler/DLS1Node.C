/* $Id: DLS1Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSA1Node.H"
#include "DLS1Node.H"


DLS1Node::DLS1Node (int nid, sched_flops flops)
    : LSA1Node (nid, flops)
{
  b_level = NONE;
  dyn_t_level = NONE;
}


void
DLS1Node::ComputePrio (void)
{
  ComputeBLevel();
  ComputeDynTLevel();
}

sched_time
DLS1Node::GetPrio (void) const
{
  return b_level - dyn_t_level;
}


sched_time
DLS1Node::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PDLS1Node succ = (PDLS1Node) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


sched_time
DLS1Node::ComputeDynTLevel ()
{
  if (dyn_t_level != NONE)
    return dyn_t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PDLS1Node pred = (PDLS1Node) in_edge->GetFromNode();

    sched_time t_lev = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_lev > dyn_t_level)
      dyn_t_level = t_lev;
  }

  return dyn_t_level;
}


bool
DLS1Node::LessPrioThan (PHeapItem item) const
{
  PDLS1Node the_other = (PDLS1Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
DLS1Node::MorePrioThan (PHeapItem item) const
{
  PDLS1Node the_other = (PDLS1Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
DLS1Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
DLS1Node::PrintVoid (void) const
{
  printf ("      ");
}
