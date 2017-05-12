/* $Id: LSA2Node.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSA2NODE_H
#define __LSA2NODE_H

#include "LSANode.H"


typedef class LSA2Node *PLSA2Node;

class LSA2Node :
  public LSANode
{
  public:
    LSA2Node (int nid, sched_flops flops)
        : LSANode (nid, flops)
    {}

    void SetProcLastMsg (void);
};


#endif
