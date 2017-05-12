/* $Id: ProcList.H,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#ifndef __PROCLIST_H
#define __PROCLIST_H

#include "../data/List.H"
#include "../dag/DAGNode.H"
#include "ProcListItem.H"


typedef class ProcList *PProcList;

class ProcList :
  public List
{
  public:
    PProcListItem AddInFront (PDAGNode node);
    PProcListItem AddToBack (PDAGNode node);

    void Print (void) const;
};

#endif
