/* $Id: DSA_Node.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef _A_DSA_NODE_H
#define _A_DSA_NODE_H

#include "CP_Node.H"


class DSA_Node
  : public CP_Node, public ListItem
{
  public:
    DSA_Node (int nid, long flops);

    void Print (void) const;
};

#endif
