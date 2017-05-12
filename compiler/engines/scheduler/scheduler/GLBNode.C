/* $Id: GLBNode.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include "../dag/CMrgDAGNode.H"
#include "GLBNode.H"


GLBNode::GLBNode (int nid, sched_flops flops)
    : CMrgDAGNode (nid, flops)
{
}
