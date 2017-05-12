/* $Id: DSA_Node.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#include <stdio.h>

#include "DSA_Node.H"


DSA_Node::DSA_Node (int nid, long flops)
    : CP_Node (nid, flops), ListItem()
{
}

void
DSA_Node::Print (void) const
{
  printf ("%d ", nid);
}

