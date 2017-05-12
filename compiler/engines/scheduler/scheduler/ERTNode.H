/* $Id: ERTNode.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __ERTNODE_H
#define __ERTNODE_H

#include "../data/ListItem.H"
#include "../dag/NoDupDAGNode.H"


typedef class ERTNode *PERTNode;

class ERTNode
  : public NoDupDAGNode, public ListItem
{
  public:
    ERTNode (int nid, long flops);
    ~ERTNode () {};

    void Print (void) const;

  private:
};

#endif
