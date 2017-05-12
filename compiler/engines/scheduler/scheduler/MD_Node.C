/* $Id: MD_Node.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "MD_Node.H"


MD_Node::MD_Node (int nid, sched_flops flops)
    : LSANode (nid, flops)
{
  AEST = NONE;
  ALST = NONE;
}


void
MD_Node::ComputePrio (void)
{
  ComputeALST();
  ComputeAEST();
}

sched_time
MD_Node::GetPrio (void) const
{
  return - (ALST - AEST) / GetMeanExecTime();
}


sched_time
MD_Node::ComputeAEST (void)
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
    PMD_Node pred_node = (PMD_Node) in_edge->GetFromNode();
      
    sched_time tmp_AEST = pred_node->ComputeAEST() +
                          pred_node->GetMeanExecTime() +
                          in_edge->GetCommTime();

    if (tmp_AEST > AEST)
      AEST = tmp_AEST;
  }

  return AEST;
}  


sched_time
MD_Node::ComputeALST ()
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
    PMD_Node succ_node = (PMD_Node) out_edge->GetToNode();

    sched_time tmp_ALST = succ_node->ComputeALST() - GetMeanExecTime() -
                          out_edge->GetCommTime();

    if ((ALST == NONE) || (tmp_ALST < ALST))
      ALST = tmp_ALST;
  }

  return ALST;
}


bool
MD_Node::LessPrioThan (PHeapItem item) const
{
  PMD_Node the_other = (PMD_Node) item;

  if (GetPrio() < the_other->GetPrio())
    return true;

  return false;
} 

bool
MD_Node::MorePrioThan (PHeapItem item) const
{
  PMD_Node the_other = (PMD_Node) item;

  if (GetPrio() > the_other->GetPrio())
    return true;

  return false;
} 


void
MD_Node::Print (void) const
{
  printf ("%2d[%5.2f]", nid, GetPrio());
}

void
MD_Node::PrintVoid (void) const
{
  printf ("      ");
}
