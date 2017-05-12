/* $Id: SCFET1Node.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSA1Node.H"
#include "SCFET1Node.H"


SCFET1Node::SCFET1Node (int nid, sched_flops flops)
    : LSA1Node (nid, flops)
{
  t_level = NONE;
}


void
SCFET1Node::ComputePrio (void)
{
  ComputeTLevel();
}

sched_time
SCFET1Node::GetPrio (void) const
{
  return -t_level;
}


sched_time
SCFET1Node::ComputeTLevel ()
{
  if (t_level != NONE)
    return t_level;

  if (GetNrInEdges() == 0)
    return 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PSCFET1Node pred = (PSCFET1Node) in_edge->GetFromNode();

    sched_time t_lev = pred->ComputeTLevel() + pred->GetMeanExecTime() +
                       in_edge->GetCommTime();

    if (t_lev > t_level)
      t_level = t_lev;
  }

  return t_level;
}


bool
SCFET1Node::LessPrioThan (PHeapItem item) const
{
  PSCFET1Node the_other = (PSCFET1Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
SCFET1Node::MorePrioThan (PHeapItem item) const
{
  PSCFET1Node the_other = (PSCFET1Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
SCFET1Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
SCFET1Node::PrintVoid (void) const
{
  printf ("      ");
}
