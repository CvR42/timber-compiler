/* $Id: Scheduling.H,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#ifndef __SCHEDULING_H
#define __SCHEDULING_H

#include <stdlib.h>

#include "../dag/DAG.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../dag/PrioDAGNode.H"


typedef class Scheduling *PScheduling;


extern PScheduling sched;

extern PDAG dag;
extern PProcessors proc;
extern PProcessors old_proc;


class Scheduling
{
  public:
    Scheduling (int v_task_ordering_alg = RCP);
    virtual ~Scheduling () {};

    virtual void Schedule (void);

//    void Display (void);

  protected:
    int schedule_type;
    int task_ordering_alg;

    void OrderTasks (void);
    void OrderTasks_EST (void);
    void OrderTasks_Level (void);
    void OrderTasks_RCP (void);
    void UpdateProc (int pid, PHeap *ready_tasks_on_proc,
                     PHeap proc_list, PPrioItem *proc_item);
    sched_time ComputeStartTime (PPrioDAGNode node);
};

#endif

