/* $Id: HLFETNode.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "HLFETNode.H"


HLFETNode::HLFETNode (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  b_level = NONE;
}


void
HLFETNode::ComputePrio (void)
{
  ComputeBLevel();
}

sched_time
HLFETNode::GetPrio (void) const
{
  return b_level;
}


sched_time
HLFETNode::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PHLFETNode succ = (PHLFETNode) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


bool
HLFETNode::LessPrioThan (PHeapItem item) const
{
  PHLFETNode the_other = (PHLFETNode) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
HLFETNode::MorePrioThan (PHeapItem item) const
{
  PHLFETNode the_other = (PHLFETNode) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
HLFETNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
HLFETNode::PrintVoid (void) const
{
  printf ("      ");
}
