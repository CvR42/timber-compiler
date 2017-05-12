/* $Id: SCFET2Node.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "SCFET2Node.H"


SCFET2Node::SCFET2Node (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  t_level = NONE;
}


void
SCFET2Node::ComputePrio (void)
{
  ComputeTLevel();
}

sched_time
SCFET2Node::GetPrio (void) const
{
  return -t_level;
}


sched_time
SCFET2Node::ComputeTLevel ()
{
  if (t_level != NONE)
    return t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PSCFET2Node pred = (PSCFET2Node) in_edge->GetFromNode();

    sched_time t_lev = pred->ComputeTLevel() + pred->GetMeanExecTime() +
                       in_edge->GetCommTime();

    if (t_lev > t_level)
      t_level = t_lev;
  }

  return t_level;
}


bool
SCFET2Node::LessPrioThan (PHeapItem item) const
{
  PSCFET2Node the_other = (PSCFET2Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
SCFET2Node::MorePrioThan (PHeapItem item) const
{
  PSCFET2Node the_other = (PSCFET2Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
SCFET2Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
SCFET2Node::PrintVoid (void) const
{
  printf ("      ");
}
