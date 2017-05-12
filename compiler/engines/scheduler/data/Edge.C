/* $Id: Edge.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include "Edge.H"
#include "Node.H"



Edge::Edge (PNode f_node, PNode t_node)
{
  from_node = f_node;
  to_node = t_node;
}


PNode
Edge::GetFromNode (void) const
{
  return from_node;
}

PNode
Edge::GetToNode (void) const
{
  return to_node;
}
