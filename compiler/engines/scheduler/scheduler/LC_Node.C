/* $Id: LC_Node.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "../data/Node.H"
#include "../dag/DAGNode.H"
#include "../dag/DAGEdge.H"
#include "../dag/PrioDAGNode.H"
#include "LC_Node.H"


LC_Node::LC_Node (int nid, sched_flops flops)
    : PrioDAGNode (nid, flops)
{
  b_level = NONE;
}


   /** Node level is the length of the longest path from the current
       node to an exit node including communication.
       Depending on the parameter, the already mapped tasks
       are considered or not. */
sched_time
LC_Node::ComputeBLevel (bool consider_mapped_tasks)
{
  if ((consider_mapped_tasks == false) && (proc_id != NONE))
    return NONE;

  if (b_level != NONE)
    return b_level;

  b_level = 0;

  for (unsigned int i = 0; i < nr_out_edges; i++)
  {
    PDAGEdge edge = (PDAGEdge) GetOutEdge (i);
    PLC_Node succ = (PLC_Node) edge->GetToNode();

    if ((consider_mapped_tasks == false) && (succ->GetProcID() != NONE))
      continue;

    sched_time tmp_b_level = succ->ComputeBLevel (consider_mapped_tasks);

    if ((GetProcID() == NONE) || (GetProcID() != succ->GetProcID()))
      tmp_b_level += edge->GetCommTime();

    if (tmp_b_level > b_level)
      b_level = tmp_b_level;
  }

  b_level += GetMeanExecTime();
  return b_level;
}

void
LC_Node::ResetBLevel (void)
{
  b_level = NONE;
}

sched_time
LC_Node::GetBLevel (void) const
{
  return b_level;
}


bool
LC_Node::LessPrioThan (PHeapItem item) const
{
  PLC_Node the_other = (PLC_Node) item;

  if (b_level < the_other->b_level)
    return true;

  return false;
} 

bool
LC_Node::MorePrioThan (PHeapItem item) const
{
  PLC_Node the_other = (PLC_Node) item;

  if (b_level > the_other->b_level)
    return true;

  return false;
} 


void
LC_Node::Print (void) const
{
  printf ("%2d[%4.1f]", nid, b_level);
}

void
LC_Node::PrintVoid (void) const
{
  printf ("       ");
}

