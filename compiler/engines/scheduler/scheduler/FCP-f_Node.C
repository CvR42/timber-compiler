/* $Id: FCP-f_Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/PrioDAGNode.H"
#include "../dag/DAGEdge.H"
#include "FCP-f_Node.H"


FCP_f_Node::FCP_f_Node (int nid, long flops)
    : PrioDAGNode (nid, flops)
{
  b_level = NONE;
}

  
   /* Calculate bottom levels by bottom-up topological traversing of the DAG */
sched_time
FCP_f_Node::ComputeBLevel (void)
{
  if (b_level != NONE)
    return b_level;

  b_level = GetMeanExecTime();
  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PFCP_f_Node succ_node = (PFCP_f_Node) out_edge->GetToNode();
    sched_time level = succ_node->ComputeBLevel() + out_edge->GetCommTime() +
                       GetMeanExecTime();

    if (level > b_level)
      b_level = level;
  }

  return b_level;
}


sched_time
FCP_f_Node::GetPrio (void) const
{
  return b_level;
}


bool
FCP_f_Node::LessPrioThan (PHeapItem item) const
{
  PFCP_f_Node node = (PFCP_f_Node) item;

  if (GetPrio() <= node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() < node->GetNrOutEdges())
      return true;

  return false;
}


bool
FCP_f_Node::MorePrioThan (PHeapItem item) const
{
  PFCP_f_Node node = (PFCP_f_Node) item;

  if (GetPrio() > node->GetPrio())
    return true;

  if (GetPrio() == node->GetPrio())
    if (GetNrOutEdges() > node->GetNrOutEdges())
      return true;

  return false;
}


void
FCP_f_Node::Print (void) const
{
  printf ("%2d[%7.1f]", nid, b_level);
}


void
FCP_f_Node::PrintVoid (void) const
{
  printf ("           ");
}

