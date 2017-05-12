/* $Id: PrioItem.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdio.h>

#include "Sched.H"
#include "HeapItem.H"
#include "PrioItem.H"


PrioItem::PrioItem (int idx, double prio1, double prio2)
    : HeapItem()
{
  this->idx = idx;
  this->prio1 = prio1;
  this->prio2 = prio2;
}


bool
PrioItem::LessPrioThan (PHeapItem item) const
{
  PPrioItem the_other = (PPrioItem) item;

  if (prio1 < the_other->prio1)
    return true;
  else if (prio1 == the_other->prio1)
    if (prio2 < the_other->prio2)
      return true;

  return false;
}

bool
PrioItem::MorePrioThan (PHeapItem item) const
{
  PPrioItem the_other = (PPrioItem) item;

  if (prio1 > the_other->prio1)
    return true;
  else if (prio1 == the_other->prio1)
    if (prio2 > the_other->prio2)
      return true;

  return false;
}


int
PrioItem::GetIDX (void) const
{
  return idx;
}

void
PrioItem::SetIDX (int x)
{
  idx = x;
}


void
PrioItem::SetPrio (double val)
{
  prio1 = val;
}

void
PrioItem::AddToPrio (double val)
{
  prio1 += val;
}

double
PrioItem::GetPrio (void) const
{
  return prio1;
}


void
PrioItem::Print (void) const
{
  printf ("%2d(%5.2f)", idx, prio1);
}

void
PrioItem::PrintVoid (void) const
{
  printf ("         ");
}


bool
PrioItem::less (const PPrioItem &item1, const PPrioItem &item2)
{
  if (item1->prio1 <= item2->prio1)
    return true;
  else if (item1->prio1 == item2->prio1)
    if (item1->prio2 <= item2->prio2)
      return true;

  return false;
}
