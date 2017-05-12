/* $Id: HeapItem.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __HEAPITEM_H
#define __HEAPITEM_H

#include "Sched.H"


typedef class HeapItem *PHeapItem;

class HeapItem
{
  public:
    HeapItem (void);
    virtual ~HeapItem () {};

    virtual bool LessPrioThan (PHeapItem the_other_item) const = 0;
    virtual bool MorePrioThan (PHeapItem the_other_item) const = 0;

    void SetIndexInHeap (int val);
    int GetIndexInHeap (void) const;

    virtual void Print (void) const = 0;
    virtual void PrintVoid (void) const = 0;

  protected:
    int index_in_heap;  // Index in the array modeling the heap
                        //  - used to quickly delete this from heap
};

#endif
