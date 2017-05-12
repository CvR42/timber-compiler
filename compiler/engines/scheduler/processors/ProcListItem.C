/* $Id: ProcListItem.C,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#include <stdio.h>

#include "../data/ListItem.H"
#include "../data/Sched.H"
#include "../dag/DAG.H"
#include "../scheduler/Scheduling.H"
#include "ProcListItem.H"


ProcListItem::ProcListItem (PDAGNode node)
    : ListItem()
{
  crt_nid = node->GetID();

  start_time = NONE;
  finish_time = NONE;
}

ProcListItem::ProcListItem (PDAGNode node,
                            PProcListItem prev, PProcListItem next)
    : ListItem (prev, next)
{
  crt_nid = node->GetID();

  start_time = NONE;
  finish_time = NONE;
}


PDAGNode
ProcListItem::GetCrtNode (void) const
{
  return dag->GetNode (crt_nid);
}


void
ProcListItem::SetStartTime (sched_time val)
{
  start_time = val;
}

sched_time
ProcListItem::GetStartTime (void) const
{
  return start_time;
}


void
ProcListItem::SetFinishTime (sched_time val)
{
  finish_time = val;
}

sched_time
ProcListItem::GetFinishTime (void) const
{
  return finish_time;
}

  
void
ProcListItem::InitForSimulation (void)
{
  unsched_pred = dag->GetNode(crt_nid)->GetNrInEdges();
  start_time = 0;
  finish_time = 0;    
}

int
ProcListItem::GetNrUnschedPred (void) const
{
  return unsched_pred;
}

void
ProcListItem::DecNrUnschedPred (void)
{
  unsched_pred--;
}


void
ProcListItem::Print (void) const
{
  printf ("%d ", crt_nid);
}
