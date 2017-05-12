/* $Id: PrioDAGNode.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../dag/NoDupDAGNode.H"
#include "../dag/DAGEdge.H"
#include "PrioDAGNode.H"


PrioDAGNode::PrioDAGNode (int nid, long flops)
    : NoDupDAGNode (nid, flops),  HeapItem()
{
  order_prio = NONE;
}
  
  
sched_time
PrioDAGNode::ComputeOrderPrio (void)
{
  if (order_prio != NONE)
    return order_prio;

  order_prio = GetExecTime (proc_id);

  for (unsigned int i = 0; i < GetNrOutEdges(); i++)
  {
    PDAGEdge edge = (PDAGEdge) GetOutEdge (i);
    PPrioDAGNode succ = (PPrioDAGNode) edge->GetToNode();
    sched_time tmp_prio = succ->ComputeOrderPrio() + GetExecTime (proc_id);

    tmp_prio += edge->GetCommTime (proc_id, succ->proc_id);

    if (tmp_prio > order_prio)
      order_prio = tmp_prio;
  }

  return order_prio;
}


void
PrioDAGNode::SetOrderPrio (sched_time val)
{
  order_prio = val;
}


sched_time
PrioDAGNode::GetOrderPrio (void) const
{
  return order_prio;
}


bool
PrioDAGNode::LessPrioThan (PHeapItem item) const
{
  PPrioDAGNode the_other = (PPrioDAGNode) item;

  if (order_prio < the_other->order_prio)
    return true;
  else
    if (order_prio == the_other->order_prio)
      if (level > the_other->level)
        return true;

  return false;
} 

bool
PrioDAGNode::MorePrioThan (PHeapItem item) const
{
  PPrioDAGNode the_other = (PPrioDAGNode) item;

  if (order_prio > the_other->order_prio)
    return true;
  else
    if (order_prio == the_other->order_prio)
      if (level < the_other->level)
        return true;

  return false;
}


void
PrioDAGNode::Print (void) const
{
  printf ("%2d[%4.1f]", nid, GetOrderPrio());
}

void
PrioDAGNode::PrintVoid (void) const
{
  printf ("        ");
}
