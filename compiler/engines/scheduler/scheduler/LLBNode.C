/* $Id: LLBNode.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "../data/arguments.h"
#include "../data/Sched.H"
#include "../dag/DAGEdge.H"
#include "LLBNode.H"


LLBNode::LLBNode (int nid, long flops)
    : CMrgDAGNode (nid, flops)
{
  t_level = 0;
  b_level = NONE;
}

  
void
LLBNode::SetTLevel (sched_time val)
{
  t_level = val;
}

sched_time
LLBNode::GetTLevel (void) const
{
  return t_level;
}


   /* Calculate bottom levels by bottom-up topological traversing of the DAG */
sched_time
LLBNode::ComputeBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PLLBNode succ_node = (PLLBNode) out_edge->GetToNode();
    sched_time level = succ_node->ComputeBLevel() + GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}


sched_time
LLBNode::GetPrio (void) const
{
  switch (sched_alg)
  {
    case DSC:
      if (cluster_merging_alg == LLB)
        return b_level;
      else
      {
        fprintf (stderr, "LLBNode::GetPrio(): wrong algorithm: %d\n",
                 sched_alg);
        exit (1);
      }
    case CPM:
      return b_level;
    case WL:
      return GetMeanExecTime();
    default:
      fprintf (stderr, "LLBNode::GetPrio(): wrong algorithm: %d\n", sched_alg);
      exit (1);
  }
  return -1;
}


bool
LLBNode::LessPrioThan (PHeapItem item) const
{
  PLLBNode node = (PLLBNode) item;

  if (GetPrio() > node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() < node->GetNrOutEdges())
      return true;

  return false;
}


bool
LLBNode::MorePrioThan (PHeapItem item) const
{
  PLLBNode node = (PLLBNode) item;

  if (GetPrio() < node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() > node->GetNrOutEdges())
      return true;

  return false;
}


void
LLBNode::Print (void) const
{
  printf ("%2d[%7.1f]", nid, t_level);
}


void
LLBNode::PrintVoid (void) const
{
  printf ("           ");
}

