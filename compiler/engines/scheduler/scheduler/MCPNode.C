/* $Id: MCPNode.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "../dag/PrioDAGNode.H"
#include "MCPNode.H"


MCPNode::MCPNode (int nid, sched_flops flops)
    : PrioDAGNode (nid, flops)
{
  AEST = NONE;
  ALST = NONE;
}


sched_time
MCPNode::ComputeAEST (void)
{
  if (AEST != NONE)
    return AEST;

  if (GetNrInEdges() == 0)
  {
    AEST = 0;
    return AEST;
  }

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PMCPNode pred_node = (PMCPNode) in_edge->GetFromNode();
      
    sched_time tmp_AEST = pred_node->ComputeAEST() +
                          pred_node->GetMeanExecTime() +
                          in_edge->GetCommTime();

    if (tmp_AEST > AEST)
      AEST = tmp_AEST;
  }

  return AEST;
}  


sched_time
MCPNode::ComputeALST ()
{
  if (ALST != NONE)
    return ALST;

  if (GetNrOutEdges() == 0)
  {
    ALST = ComputeAEST();
    return ALST;
  }

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge out_edge = (PDAGEdge) GetOutEdge (i);
    PMCPNode succ_node = (PMCPNode) out_edge->GetToNode();

    sched_time tmp_ALST = succ_node->ComputeALST() - GetMeanExecTime() -
                          out_edge->GetCommTime();

    if ((ALST == NONE) || (tmp_ALST < ALST))
      ALST = tmp_ALST;
  }

  return ALST;
}


bool
MCPNode::LessPrioThan (PHeapItem item) const
{
  PMCPNode the_other = (PMCPNode) item;

  if (ALST > the_other->ALST)
    return true;
  else
    if (ALST == the_other->ALST)
      if (level > the_other->level)
        return true;

  return false;
} 

bool
MCPNode::MorePrioThan (PHeapItem item) const
{
  PMCPNode the_other = (PMCPNode) item;

  if (ALST < the_other->ALST)
    return true;
  else
    if (ALST == the_other->ALST)
      if (level < the_other->level)
        return true;

  return false;
} 


void
MCPNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, ALST);
}

void
MCPNode::PrintVoid (void) const
{
  printf ("      ");
}
