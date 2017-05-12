/* $Id: List.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __LIST_H
#define __LIST_H

typedef class List *PList;

#include "ListItem.H"

class List
{
  public:
    List (void);
    ~List ();

    void AddInFront (PListItem new_item);
    void AddToBack (PListItem new_item);

    void RemoveFirst (void);
    void RemoveLast (void);
    void RemoveItem (PListItem item);

    PListItem DequeueFirst (void);
    PListItem DequeueLast (void);

    PListItem GetFirst (void) const;
    PListItem GetLast (void) const;

    void Print (void) const;

  private:
    PListItem first;
    PListItem last;
};

#endif
