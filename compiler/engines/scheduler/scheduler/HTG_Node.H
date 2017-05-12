/* $Id: HTG_Node.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __HTG_NODE_H
#define __HTG_NODE_H

#include <vector.h>

typedef class HTG_Node *PHTG_Node;

#include "../dag/DAGNode.H"


class HTG_Node
{
  public:
    HTG_Node (void) {};
    virtual ~HTG_Node () {};

    virtual PSPGNode GetEntrySPGNode (void) const = 0;
    virtual PSPGNode GetExitSPGNode (void) const = 0;
    virtual bool IsEntrySPGNode (const PSPGNode node) const = 0;
    virtual bool IsExitSPGNode (const PSPGNode node) const = 0;
    virtual unsigned int GetNrEntrySPGNode (void) const = 0;
    virtual unsigned int GetNrExitSPGNode (void) const = 0;

  protected:
};

#endif
