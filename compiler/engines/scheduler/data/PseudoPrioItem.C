/* $Id: PseudoPrioItem.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdio.h>

#include "PseudoHeapItem.H"
#include "PseudoPrioItem.H"


PseudoPrioItem::PseudoPrioItem (int idx, double prio)
    : PseudoHeapItem()
{
  this->idx = idx;
  this->prio = prio;
}


bool
PseudoPrioItem::LessPrioThan (PHeapItem item) const
{
  PPseudoPrioItem the_other = (PPseudoPrioItem) item;

  if (prio < the_other->prio)
    return true;
  else
    return false;
}

bool
PseudoPrioItem::MorePrioThan (PHeapItem item) const
{
  PPseudoPrioItem the_other = (PPseudoPrioItem) item;

  if (prio > the_other->prio)
    return true;
  else
    return false;
}


int
PseudoPrioItem::GetIDX (void) const
{
  return idx;
}


double
PseudoPrioItem::GetPrio (void) const
{
  return prio;
}


void
PseudoPrioItem::Print (void) const
{
  printf ("%2d(%5.2f)", idx, prio);
}

void
PseudoPrioItem::PrintVoid (void) const
{
  printf ("         ");
}
