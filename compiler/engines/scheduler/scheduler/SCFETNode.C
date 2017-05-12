/* $Id: SCFETNode.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "SCFETNode.H"


SCFETNode::SCFETNode (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  t_level = NONE;
}


void
SCFETNode::ComputePrio (void)
{
  ComputeTLevel();
}

sched_time
SCFETNode::GetPrio (void) const
{
  return -t_level;
}


sched_time
SCFETNode::ComputeTLevel ()
{
  if (t_level != NONE)
    return t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PSCFETNode pred = (PSCFETNode) in_edge->GetFromNode();

    sched_time t_lev = pred->ComputeTLevel() + pred->GetMeanExecTime() +
                       in_edge->GetCommTime();

    if (t_lev > t_level)
      t_level = t_lev;
  }

  return t_level;
}


bool
SCFETNode::LessPrioThan (PHeapItem item) const
{
  PSCFETNode the_other = (PSCFETNode) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
SCFETNode::MorePrioThan (PHeapItem item) const
{
  PSCFETNode the_other = (PSCFETNode) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
SCFETNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
SCFETNode::PrintVoid (void) const
{
  printf ("      ");
}
