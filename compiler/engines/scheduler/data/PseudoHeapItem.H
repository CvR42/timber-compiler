/* $Id: PseudoHeapItem.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __PSEUDOHEAPITEM_H
#define __PSEUDOHEAPITEM_H

#include "HeapItem.H"
#include "ListItem.H"


typedef class PseudoHeapItem *PPseudoHeapItem;

class PseudoHeapItem
  : public HeapItem, public ListItem
{
  public:
    PseudoHeapItem (void);
    virtual ~PseudoHeapItem () {};
};

#endif
