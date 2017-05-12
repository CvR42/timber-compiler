/* $Id: DPSNode.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "../dag/PrioDAGNode.H"
#include "DPSNode.H"


DPSNode::DPSNode (int nid, sched_flops flops)
    : PrioDAGNode (nid, flops)
{
  top_level = NONE;
  in_task_list = false;
}


sched_time
DPSNode::ComputeTopLevel (void)
{
  if (top_level != NONE)
    return top_level;

  if (GetNrInEdges() == 0)
  {
    top_level = 0;
    return top_level;
  }

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PDPSNode pred_node = (PDPSNode) in_edge->GetFromNode();
      
    sched_time tmp_TL = pred_node->ComputeTopLevel() +
                        pred_node->GetMeanExecTime() +
                        in_edge->GetCommTime();

    if (tmp_TL > top_level)
      top_level = tmp_TL;
  }

  return top_level;
}


void
DPSNode::SetInTaskList (void)
{
  in_task_list = true;
}

bool
DPSNode::InTaskList (void) const
{
  return in_task_list;
}


bool
DPSNode::LessPrioThan (PHeapItem item) const
{
  PDPSNode the_other = (PDPSNode) item;

  if ((top_level + GetMeanExecTime()) <
      (the_other->top_level + the_other->GetMeanExecTime()))
    return true;

  return false;
} 

bool
DPSNode::MorePrioThan (PHeapItem item) const
{
  PDPSNode the_other = (PDPSNode) item;

  if ((top_level + GetMeanExecTime()) >
      (the_other->top_level + the_other->GetMeanExecTime()))
    return true;

  return false;
} 


void
DPSNode::Print (void) const
{
  printf ("%2d[%5.2f]", nid, top_level);
}

void
DPSNode::PrintVoid (void) const
{
  printf ("      ");
}
