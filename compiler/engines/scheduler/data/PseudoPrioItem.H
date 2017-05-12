/* $Id: PseudoPrioItem.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __PSEUDOPRIOITEM_H
#define __PSEUDOPRIOITEM_H

#include "PseudoHeapItem.H"


typedef class PseudoPrioItem *PPseudoPrioItem;

class PseudoPrioItem
  : public PseudoHeapItem
{
  public:
    PseudoPrioItem (int idx, double prio);
    ~PseudoPrioItem () {};

    bool LessPrioThan (PHeapItem the_other_item) const;
    bool MorePrioThan (PHeapItem the_other_item) const;

    int GetIDX (void) const;

    double GetPrio (void) const;

    void Print (void) const;
    void PrintVoid (void) const;

  private:
    int idx;
    double prio;
};

#endif
