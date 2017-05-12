/* $Id: HLFET1Node.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSA1Node.H"
#include "HLFET1Node.H"


HLFET1Node::HLFET1Node (int nid, sched_flops flops)
    : LSA1Node (nid, flops)
{
  b_level = NONE;
}


void
HLFET1Node::ComputePrio (void)
{
  ComputeBLevel();
}

sched_time
HLFET1Node::GetPrio (void) const
{
  return b_level;
}


sched_time
HLFET1Node::ComputeBLevel ()
{
  if (b_level != NONE)
    return b_level;

  if (GetNrOutEdges() == 0)
    return GetMeanExecTime();

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PHLFET1Node succ = (PHLFET1Node) out_edge->GetToNode();

    sched_time b_lev = succ->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (b_lev > b_level)
      b_level = b_lev;
  }

  return b_level;
}


bool
HLFET1Node::LessPrioThan (PHeapItem item) const
{
  PHLFET1Node the_other = (PHLFET1Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
HLFET1Node::MorePrioThan (PHeapItem item) const
{
  PHLFET1Node the_other = (PHLFET1Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
HLFET1Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
HLFET1Node::PrintVoid (void) const
{
  printf ("      ");
}
