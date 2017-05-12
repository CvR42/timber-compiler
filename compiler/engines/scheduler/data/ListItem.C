/* $Id: ListItem.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdlib.h>

#include "ListItem.H"


ListItem::ListItem (void)
{
  prev = NULL;
  next = NULL;
}

ListItem::ListItem (PListItem prev, PListItem next)
{
  this->prev = prev;
  this->next = next;
}


void
ListItem::SetPrevItem (PListItem prev)
{
  this->prev = prev;
}

PListItem
ListItem::GetPrevItem (void) const
{
  return prev;
}


void
ListItem::SetNextItem (PListItem next)
{
  this->next = next;
}

PListItem
ListItem::GetNextItem (void) const
{
  return next;
}
