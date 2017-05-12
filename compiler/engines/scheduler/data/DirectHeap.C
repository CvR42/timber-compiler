/* $Id: DirectHeap.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include "DirectHeap.H"


DirectHeap::DirectHeap (int size)
    : Heap (size)
{
  ref = new PPrioItem [size];
  for (int i = 0; i < size; i++)
  {
    ref[i] = new PrioItem (i, 0);
    Heap::Enqueue (ref[i]);
  }
}

DirectHeap::~DirectHeap ()
{
  delete ref;
}


PPrioItem
DirectHeap:: GetItem (int k) const
{
  return ref[k];
}


   /* Propagate the specified element, until the heap conditions are satisfied.
      As long as one of the children has a higher priority, exchange its
      place with the children place and continue from the new place.
   */
void
DirectHeap::PropagateDown (int x)
{
  int new_x = Heap::PropagateDown (x);
  PPrioItem aux = ref[new_x];

  for (int i = new_x; i != x; i = (i-1)/2)
    ref[i] = ref[(i-1)/2];
  ref [x] = aux;
}


   /* Propagate the specified element, until the heap conditions are satisfied.
      As long as the parent has a lower priority, exchange its
      place with the parent place and continue from the new place.
   */
void
DirectHeap::PropagateUp (int x)
{
  int new_x = Heap::PropagateUp (x);
  PPrioItem aux = ref[x];

  for (int i = x; i != new_x; i = (i-1)/2)
    ref[i] = ref[(i-1)/2];
  ref [new_x] = aux;
}
