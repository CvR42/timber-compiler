/* $Id: WCMNode.H,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#ifndef __WCMNODE_H
#define __WCMNODE_H

typedef class WCMNode *PWCMNode;

class WCMNode
  : public CMrgDAGNode
{
  public:
    WCMNode (int nid, int flops);

    void SetEST (int val);
    int GetEST (void) const;

  private:
    int earliest_start_time;
  
};

#endif
