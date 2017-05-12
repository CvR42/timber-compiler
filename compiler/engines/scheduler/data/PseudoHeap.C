/* $Id: PseudoHeap.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdio.h>

#include "List.H"
#include "Heap.H"
#include "PseudoHeapItem.H"
#include "PseudoHeap.H"


PseudoHeap::PseudoHeap (int size)
{
  if (size > 0)
    this->size = size;
  else
    this->size = 1;

  list = new List();
  heap = new Heap (this->size);
}

PseudoHeap::~PseudoHeap ()
{
  delete list;
  delete heap;
}


PPseudoHeapItem
PseudoHeap::Head (void) const
{
  return (PPseudoHeapItem) heap->Head();
}


PPseudoHeapItem
PseudoHeap::Dequeue (void)
{
  PPseudoHeapItem item = (PPseudoHeapItem) heap->Dequeue();

  PPseudoHeapItem aux_item = (PPseudoHeapItem) list->DequeueFirst();
  if (aux_item != NULL)
    heap->Enqueue (aux_item);

  return item;
}

void
PseudoHeap::Enqueue (PPseudoHeapItem item)
{
  if (heap->GetSize() < size)
    heap->Enqueue (item);
  else
    list->AddToBack (item);
}


PPseudoHeapItem
PseudoHeap::RemoveItem (PPseudoHeapItem item)
{
  PPseudoHeapItem del_item;

  if ((del_item = (PPseudoHeapItem) heap->RemoveItem (item)) == NULL)
    list->RemoveItem (item);
  else
  {
    PPseudoHeapItem aux_item = (PPseudoHeapItem) list->DequeueFirst();
    if (aux_item != NULL)
      heap->Enqueue (aux_item);
  }

  return del_item;
}


void
PseudoHeap::Print (void) const
{
  heap->Print();
  puts ("\n--\n");
  list->Print();
}

