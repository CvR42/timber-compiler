/* $Id: CMrgDAGNode.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "DAGEdge.H"
#include "PrioDAGNode.H"
#include "CMrgDAGNode.H"


CMrgDAGNode::CMrgDAGNode (int nid, long flops)
    : PrioDAGNode (nid, flops)
{
  virtual_processor = NONE;
  old_start_time = 0;
  old_finish_time = 0;
}


void
CMrgDAGNode::SetVirtualProcessor (int vpid)
{
  virtual_processor = vpid;
}

int
CMrgDAGNode::GetVirtualProcessor (void) const
{
  return virtual_processor;
}
