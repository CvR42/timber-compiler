/* $Id: ETFNode.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __ETFNODE_H
#define __ETFNODE_H

#include "../data/ListItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class ETFNode *PETFNode;

class ETFNode
  : public NoDupDAGNode, public ListItem
{
  public:
    ETFNode (int nid, long flops);
    ~ETFNode () {};

    void Print (void) const;

  private:
};

#endif
