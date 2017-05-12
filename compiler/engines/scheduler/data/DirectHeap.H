/* $Id: DirectHeap.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef _A_DIRECT_HEAP_H
#define _A_DIRECT_HEAP_H

#include "Heap.H"
#include "PrioItem.H"


typedef class DirectHeap *PDirectHeap;

#define INIT_BUF_SIZE  8

class DirectHeap
  : protected Heap
{
  public:
    DirectHeap (int size);
    ~DirectHeap ();

    PPrioItem Head (void) { return (PPrioItem) Heap::Head(); }
    int GetSize (void) { return Heap::GetSize(); }
    void BalanceHeap (PPrioItem item) { Heap::BalanceHeap (item); }

    PPrioItem GetItem (int k) const;

  private:
    PPrioItem *ref;

    void PropagateDown (int x);
    void PropagateUp (int x);
};

#endif
