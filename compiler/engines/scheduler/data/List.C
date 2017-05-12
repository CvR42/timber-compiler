/* $Id: List.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "ListItem.H"
#include "List.H"



List::List (void)
{
  first = NULL;
  last = NULL;
}

List::~List ()
{
  PListItem item, next_item;

  for (item = first; item != NULL; item = next_item)
  {
    next_item = item->GetNextItem();
    delete item;
  }
}


void
List::AddInFront (PListItem new_item)
{
  new_item->SetNextItem (first);
  new_item->SetPrevItem (NULL);

  if (first == NULL)
    last = new_item;
  else
    first->SetPrevItem (new_item);

  first = new_item;
}

void
List::AddToBack (PListItem new_item)
{
  new_item->SetPrevItem (last);
  new_item->SetNextItem (NULL);

  if (last == NULL)
    first = new_item;
  else
    last->SetNextItem (new_item);

  last = new_item;
}


void
List::RemoveFirst (void)
{
  if (first == NULL)
    return;

  first = first->GetNextItem();
  if (first == NULL)
    last = NULL;
  else
    first->SetPrevItem (NULL);
}

void
List::RemoveLast (void)
{
  if (last == NULL)
    return;

  last = last->GetPrevItem();
  if (last == NULL)
    first = NULL;
  else
    last->SetNextItem (NULL);
}


void
List::RemoveItem (PListItem item)
{
  if (item == GetFirst())
  {
    RemoveFirst();
    return;
  }

  if (item == GetLast())
  {
    RemoveLast();
    return;
  }
    
  item->GetPrevItem()->SetNextItem (item->GetNextItem());
  item->GetNextItem()->SetPrevItem (item->GetPrevItem());

  item->SetPrevItem (NULL);
  item->SetNextItem (NULL);
}


PListItem
List::DequeueFirst (void)
{
  PListItem item = GetFirst();
  RemoveFirst();

  return item;
}

PListItem
List::DequeueLast (void)
{
  PListItem item = GetLast();
  RemoveLast();

  return item;
}


PListItem
List::GetFirst (void) const
{
  return first;
}

PListItem
List::GetLast (void) const
{
  return last;
}


void
List::Print (void) const
{
  for (PListItem item = GetFirst(); item != NULL; item = item->GetNextItem())
    item->Print();

  putchar ('\n');
}
