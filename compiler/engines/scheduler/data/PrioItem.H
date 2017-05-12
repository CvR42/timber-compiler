/* $Id: PrioItem.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __PRIOITEM_H
#define __PRIOITEM_H

#include "HeapItem.H"


typedef class PrioItem *PPrioItem;

class PrioItem
  : public HeapItem
{
  public:
    PrioItem (int idx, double prio1, double prio2 = 0);

    bool LessPrioThan (PHeapItem the_other_item) const;
    bool MorePrioThan (PHeapItem the_other_item) const;

//    bool operator< (const PrioItem &the_other_item);

    int GetIDX (void) const;
    void SetIDX (int x);

    void SetPrio (double val);
    void AddToPrio (double val);
    double GetPrio (void) const;

    void Print (void) const;
    void PrintVoid (void) const;

    static bool less (const PPrioItem &item1, const PPrioItem &item2);

  private:
    int idx;
    double prio1, prio2;
};

#endif
