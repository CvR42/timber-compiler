/* $Id: ProcListItem.H,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#ifndef __PROC_LIST_ITEM_H
#define __PROC_LIST_ITEM_H

#include "../data/Sched.H"
#include "../data/ListItem.H"
#include "../dag/DAGNode.H"


typedef class ProcListItem *PProcListItem;

class ProcListItem :
  public ListItem
{
  public:
    ProcListItem (PDAGNode node);
    ProcListItem (PDAGNode node, PProcListItem prev, PProcListItem next);

    PDAGNode GetCrtNode (void) const;

    void SetStartTime (sched_time val);
    sched_time GetStartTime (void) const;

    void SetFinishTime (sched_time val);
    sched_time GetFinishTime (void) const;

    void InitForSimulation (void);
    int GetNrUnschedPred (void) const;
    void DecNrUnschedPred (void);

    void Print (void) const;

  private:
    int crt_nid;
    sched_time start_time;
    sched_time finish_time;
    int unsched_pred;
};

#endif
