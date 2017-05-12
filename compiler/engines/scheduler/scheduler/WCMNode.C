/* $Id: WCMNode.C,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#include "../data/Sched.H"
#include "../dag/CMrgDAGNode.H"
#include "WCMNode.H"


WCMNode::WCMNode (int nid, int flops)
    : CMrgDAGNode (nid, flops)
{
  level = NONE;
  earliest_start_time = NONE;
}


void
WCMNode::SetEST (int val)
{
  earliest_start_time = val;
}

int
WCMNode::GetEST (void) const
{
  return earliest_start_time;
}
