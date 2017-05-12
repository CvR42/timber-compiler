/* $Id: CMrgDAGNode.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __CMRGDAGNODE_H
#define __CMRGDAGNODE_H

#include "PrioDAGNode.H"


typedef class CMrgDAGNode *PCMrgDAGNode;

class CMrgDAGNode :
  public PrioDAGNode
{
  public:
    CMrgDAGNode (int nid, long flops);

    void SetVirtualProcessor (int cid);
    int GetVirtualProcessor (void) const;

  protected:
    int virtual_processor;
};

#endif
