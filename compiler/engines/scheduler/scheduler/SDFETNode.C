/* $Id: SDFETNode.C,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "SDFETNode.H"


SDFETNode::SDFETNode (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  dyn_t_level = NONE;
}


void
SDFETNode::ComputePrio (void)
{
  ComputeDynTLevel();
}

sched_time
SDFETNode::GetPrio (void) const
{
  return -dyn_t_level;
}


sched_time
SDFETNode::ComputeDynTLevel ()
{
  if (dyn_t_level != NONE)
    return dyn_t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PSDFETNode pred = (PSDFETNode) in_edge->GetFromNode();

    sched_time t_lev = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_lev > dyn_t_level)
      dyn_t_level = t_lev;
  }

  return dyn_t_level;
}


bool
SDFETNode::LessPrioThan (PHeapItem item) const
{
  PSDFETNode the_other = (PSDFETNode) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
SDFETNode::MorePrioThan (PHeapItem item) const
{
  PSDFETNode the_other = (PSDFETNode) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
SDFETNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
SDFETNode::PrintVoid (void) const
{
  printf ("      ");
}
