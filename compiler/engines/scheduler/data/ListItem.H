/* $Id: ListItem.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __LIST_ITEM_H
#define __LIST_ITEM_H

#include <stdlib.h>


typedef class ListItem *PListItem;

class ListItem
{
  public:
    ListItem (void);
    ListItem (PListItem prev, PListItem next);
    virtual ~ListItem () {};

    void SetPrevItem (PListItem prev);
    PListItem GetPrevItem (void) const;

    void SetNextItem (PListItem next);
    PListItem GetNextItem (void) const;

    virtual void Print (void) const = NULL;

  private:
    PListItem prev;
    PListItem next;
};

#endif
