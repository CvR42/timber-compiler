/* $Id: LSA2Node.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#include <stdio.h>

#include "../dag/DAGEdge.H"
#include "LSANode.H"
#include "LSA2Node.H"


void
LSA2Node::SetProcLastMsg (void)
{
  sched_time t = 0;

  for (unsigned int i = 0; i < GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) GetInEdge (i);
    PLSA2Node pred = (PLSA2Node) in_edge->GetFromNode();

    sched_time t_aux = pred->GetFinishTime() + in_edge->GetCommTime();

    if (t_aux > t)
    {
      t = t_aux;
      SetProcID (pred->GetProcID());
    }
  }
}
