/* $Id: PseudoHeap.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __PSEUDOHEAP_H
#define __PSEUDOHEAP_H

#include "List.H"
#include "Heap.H"
#include "PseudoHeapItem.H"


typedef class PseudoHeap *PPseudoHeap;

class PseudoHeap
{
  public:
    PseudoHeap (int size);
    ~PseudoHeap ();

    PPseudoHeapItem Head (void)  const;
    PPseudoHeapItem Dequeue (void);
    void Enqueue (PPseudoHeapItem item);
    PPseudoHeapItem RemoveItem (PPseudoHeapItem item);

    void Print (void) const;

  private:
    int size;
    PHeap heap;
    PList list;
};

#endif
