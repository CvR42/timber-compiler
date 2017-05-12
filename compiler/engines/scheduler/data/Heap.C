/* $Id: Heap.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdio.h>

#include "HeapItem.H"
#include "Heap.H"


Heap::Heap (int size)
{
  arr = new PHeapItem [size];
  room = size;
  sz = 0;
}

Heap::~Heap ()
{
  delete arr;
}


   /* Get the most priority element and remove it.
      The element is not removed.
   */
PHeapItem
Heap::Head (void) const
{
  if (sz == 0)
    return 0;

  return arr[0];
}


   /* Get the element with the highest priority and remove it.
      The element is removed by moving the last element to the root and
      propagating it downwards until it satisfies the heap constrains.
   */
PHeapItem
Heap::Dequeue (void)
{
  PHeapItem item;

  if (sz == 0)
    return 0;

  item = arr[0];

  arr[0]->SetIndexInHeap (NONE);
  arr[0] = arr[sz-1];
  sz--;

  if (sz > 0)
    PropagateDown (0);

  return item;
}


   /* Remove an element by moving the last element to the root and
      propagating it downwards until it satisfies the heap constrains.
   */
PHeapItem 
Heap::RemoveItem (PHeapItem item)
{
  int idx = item->GetIndexInHeap();
  if (idx == NONE)
    return 0;

  PHeapItem aux = arr[idx];
  arr[idx]->SetIndexInHeap (NONE);
  arr[idx] = arr[sz-1];
  sz--;

  if ((sz > 0) && (idx < sz))
    PropagateDown (idx);
  return aux;
}  


   /* Add a new element to the heap.
      The element is first added at the end of the heap, and then is
      propagated upwards until it satisfies the heap constrains.
   */
void
Heap::Enqueue (PHeapItem item)
{
  if (sz == room)
    AllocMoreItems();
    
  arr[sz] = item;
  sz++;
  PropagateUp (sz-1);
}

PHeapItem
Heap::GetItem (int idx) const
{
  return arr[idx];
}

int
Heap::GetSize (void) const
{
  return sz;
}


   /* If the element with index 'x' has change its priority, it will be
      propagated to a position that will satisfy the heap conditions.
   */
void
Heap::BalanceHeap (PHeapItem item)
{
  int x = item->GetIndexInHeap();
  int parent = (x-1)/2;

  if (x == NONE)
    return;

  if (x == 0)                              /* No parents => propagate down  */
  {
    PropagateDown (x);
    return;
  }

  if (arr[x]->MorePrioThan (arr[parent]))  /* if x-thelem has a higher prio */
      PropagateUp (x);                     /* -> propagate it upward        */
  else                                     /* otherwise                     */ 
    PropagateDown (x);                     /* -> propagate it down          */
}


   /* Print the subtree with root elem. 'idx'.
        - left sons are printed on the same row as their fathers
        - right sons are printed on a new row
   */
void
Heap::Print (int idx) const
{
  if (idx >= sz)
    return;

  if ((2 * (idx / 2)) == idx)
  {                                           /* right son */
    printf ("\n");

    for (int i = 2; i <= idx; i *= 2)
    {
      arr[idx]->PrintVoid();
    }
  }
  arr[idx]->Print ();

  Print (2 * idx + 1);
  Print (2 * idx + 2);
}



   /* Propagate the specified element, until the heap conditions are satisfied.
      As long as one of the children has a higher priority, exchange its
      place with the children place and continue from the new place.
   */
int
Heap::PropagateDown (int x)
{
  int child1 = 2*x+1;
  int child2 = 2*x+2;
  int child;

  if (child1 >= sz)                      /* No children => nothing to do. */
  {
    arr[x]->SetIndexInHeap (x);
    return x;
  }

  if (child2 < sz)                       /* There are 2 children. */
  {                                      /* -> Pick the more prioritar one */
    if (arr[child1]->MorePrioThan (arr[child2]))
      child = child1;
    else
      child = child2;
  }
  else
  {                                      /* There is only one child. */
    child = child1;
  }

  if (arr[x]->LessPrioThan (arr[child])) 
  {                                      /* Heap condition doesn't hold. */
    PHeapItem aux = arr[x];              /* -> Exchange element position */
    arr[x] = arr[child];                 /*    with children's position. */
    arr[child] = aux;
    arr[x]->SetIndexInHeap (x);

    PropagateDown (child);               /* Propagation from the new pos. */
    return child;
  }

  arr[x]->SetIndexInHeap (x);
  return x;
}


   /* Propagate the specified element, until the heap conditions are satisfied.
      As long as the parent has a lower priority, exchange its
      place with the parent place and continue from the new place.
   */
int
Heap::PropagateUp (int x)
{
  int parent = (x-1)/2;
    
  if (x == 0)                            /* No parents => nothing to do. */
  {
    arr[x]->SetIndexInHeap (x);
    return x;
  }

  if (arr[x]->MorePrioThan (arr[parent]))
  {                                      /* Heap condition doesn't hold. */
    PHeapItem aux = arr[x];              /* -> Exchange element position */
    arr[x] = arr[parent];                /*    with parent's position.   */
    arr[parent] = aux;
    arr[x]->SetIndexInHeap (x);

    PropagateUp (parent);                /* Propagation from the new pos. */
    return parent;
  }

  arr[x]->SetIndexInHeap (x);
  return x;
}


void
Heap::AllocMoreItems (void)
{
  PHeapItem *aux = arr;
    
  arr = new PHeapItem [2 * room];

  for (int i = 0; i < sz; i++)
  {
    arr[i] = aux[i];
  }
  for (int i = sz + 1; i < 2 * sz; i++)
  {
    arr[i] = 0;
  }

  room *= 2;

  delete aux;
}
