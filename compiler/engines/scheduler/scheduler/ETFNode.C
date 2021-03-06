/* $Id: ETFNode.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "../data/ListItem.H"
#include "../dag/NoDupDAGNode.H"
#include "ETFNode.H"


ETFNode::ETFNode (int nid, long flops)
    : NoDupDAGNode (nid, flops), ListItem ()
{
}

  
void
ETFNode::Print (void) const
{
  printf ("%d ", nid);
}

