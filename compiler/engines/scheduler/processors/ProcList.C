/* $Id: ProcList.C,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#include <stdio.h>

#include "../data/List.H"
#include "../dag/DAGNode.H"
#include "ProcListItem.H"
#include "ProcList.H"


PProcListItem
ProcList::AddInFront (PDAGNode node)
{
  PProcListItem crt = new ProcListItem (node); 
  List::AddInFront (crt);

  return crt;
}

PProcListItem
ProcList::AddToBack (PDAGNode node)
{
  PProcListItem crt = new ProcListItem (node);
  List::AddToBack (crt);

  return crt;
}


void
ProcList::Print (void) const
{
  for (PProcListItem nl = (PProcListItem) GetFirst(); 
       nl != NULL; nl = (PProcListItem) nl->GetNextItem())
  {
    printf (" - %d#", nl->GetCrtNode()->GetID());
  }
  printf ("\n");
}
