/* $Id: Heap.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __HEAP_H
#define __HEAP_H

#include "HeapItem.H"

typedef class Heap *PHeap;

#define INIT_BUF_SIZE  8

class Heap
{
  public:
    Heap (int size = INIT_BUF_SIZE);
    ~Heap ();

    PHeapItem Head (void)  const;
    PHeapItem Dequeue (void);
    void Enqueue (PHeapItem item);
    PHeapItem RemoveItem (PHeapItem item);
    PHeapItem GetItem (int idx) const;
    int GetSize (void) const;

    void BalanceHeap (PHeapItem item);

    void Print (int idx = 0) const;

  protected:
    int PropagateDown (int x);
    int PropagateUp (int x);

  private:
    int room;
    int sz;
    PHeapItem *arr;

    void AllocMoreItems (void);
};

#endif
