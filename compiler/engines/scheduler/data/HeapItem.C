/* $Id: HeapItem.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include "Sched.H"
#include "HeapItem.H"

HeapItem::HeapItem (void)
{
  index_in_heap = NONE;
}


void
HeapItem::SetIndexInHeap (int val)
{
  index_in_heap = val;
}

int
HeapItem::GetIndexInHeap (void) const
{
  return index_in_heap;
}
