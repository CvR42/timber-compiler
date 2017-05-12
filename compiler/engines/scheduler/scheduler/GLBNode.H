/* $Id: GLBNode.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __GLBNODE_H
#define __GLBNODE_H

#include "../dag/CMrgDAGNode.H"


typedef class GLBNode *PGLBNode;;

class GLBNode
  : public CMrgDAGNode
{
  public:
    GLBNode (int nid, sched_flops flops);
};


#endif
