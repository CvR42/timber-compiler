/* $Id: DSCNode.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/PrioDAGNode.H"
#include "../dag/DAGEdge.H"
#include "DSCNode.H"



DSCNode::DSCNode (int nid, sched_flops flops)
    : PrioDAGNode (nid, flops)
{
  b_level = NONE;
  t_level = 0;
  examined_parents = 0;
}


   /* Calculate bottom levels by bottom-up topological traversing of the DAG */
sched_time
DSCNode::CalcBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PDSCNode succ_node = (PDSCNode) out_edge->GetToNode();
    sched_time level = succ_node->CalcBLevel() +
                       GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}
  

void
DSCNode::ModifyTLevel (PDSCNode parent)
{
  sched_time level = parent->GetTLevel() + parent->GetMeanExecTime();
  PDAGEdge in_edge = (PDAGEdge) GetLinkFrom (parent);

  assert (in_edge != NULL);

  if (level > t_level)
    t_level = level;
}


sched_time
DSCNode::GetTLevel (void) const
{
  return t_level;
}


void
DSCNode::SetTLevel (sched_time val)
{
  t_level = val;
}


sched_time
DSCNode::GetPrio (void) const
{
  return t_level + b_level;
}


bool
DSCNode::LessPrioThan (PHeapItem item) const
{
  PDSCNode node = (PDSCNode) item;

  if (GetPrio() < node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() < node->GetNrOutEdges())
      return true;

  return false;
}


bool
DSCNode::MorePrioThan (PHeapItem item) const
{
  PDSCNode node = (PDSCNode) item;

  if (GetPrio() > node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() > node->GetNrOutEdges())
        return true;

  return false;
}


void
DSCNode::IncNrExaminedParents (void)
{
  examined_parents++;
}


bool
DSCNode::IsFree (void) const
{
  return (examined_parents == GetNrInEdges());
}


bool
DSCNode::IsNewPartialFree (void) const
{
  return (examined_parents == 1);
}


void
DSCNode::Print (void) const
{
  printf ("%2d[%7.1f,%2d]", nid, GetPrio(), GetNrOutEdges());
}

void
DSCNode::PrintVoid (void) const
{
  printf ("               ");
}
