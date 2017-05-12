/* $Id: SDFET1Node.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSA1Node.H"
#include "SDFET1Node.H"


SDFET1Node::SDFET1Node (int nid, sched_flops flops)
    : LSA1Node (nid, flops)
{
  dyn_t_level = NONE;
}


void
SDFET1Node::ComputePrio (void)
{
  ComputeDynTLevel();
}

sched_time
SDFET1Node::GetPrio (void) const
{
  return -dyn_t_level;
}


sched_time
SDFET1Node::ComputeDynTLevel ()
{
  if (dyn_t_level != NONE)
    return dyn_t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PSDFET1Node pred = (PSDFET1Node) in_edge->GetFromNode();

    sched_time t_lev = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_lev > dyn_t_level)
      dyn_t_level = t_lev;
  }

  return dyn_t_level;
}


bool
SDFET1Node::LessPrioThan (PHeapItem item) const
{
  PSDFET1Node the_other = (PSDFET1Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
SDFET1Node::MorePrioThan (PHeapItem item) const
{
  PSDFET1Node the_other = (PSDFET1Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
SDFET1Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
SDFET1Node::PrintVoid (void) const
{
  printf ("      ");
}
