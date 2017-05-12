/* $Id: Edge.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __EDGE_H
#define __EDGE_H

typedef class Edge *PEdge;

#include "Node.H"


class Edge
{
  public:
    Edge (PNode from_node, PNode to_node);

    PNode GetFromNode (void) const;
    PNode GetToNode (void) const;
    
  private:
    PNode from_node;
    PNode to_node;
};

#endif
