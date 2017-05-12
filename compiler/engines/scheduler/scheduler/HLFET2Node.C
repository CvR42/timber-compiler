/* $Id: HLFET2Node.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "HLFET2Node.H"


HLFET2Node::HLFET2Node (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  b_level = NONE;
}


void
HLFET2Node::ComputePrio (void)
{
  ComputeBLevel();
}

sched_time
HLFET2Node::GetPrio (void) const
{
  return b_level;
}


sched_time
HLFET2Node::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PHLFET2Node succ = (PHLFET2Node) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


bool
HLFET2Node::LessPrioThan (PHeapItem item) const
{
  PHLFET2Node the_other = (PHLFET2Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
HLFET2Node::MorePrioThan (PHeapItem item) const
{
  PHLFET2Node the_other = (PHLFET2Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
HLFET2Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
HLFET2Node::PrintVoid (void) const
{
  printf ("      ");
}
