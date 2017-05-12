/* $Id: FFF-f_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __FFF_f_SCHEDULING_H
#define __FFF_f_SCHEDULING_H

#include "../data/Heap.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"


class FFF_f_Scheduling
  : public Scheduling
{
  public:
    FFF_f_Scheduling (void);
    ~FFF_f_Scheduling ();

    void Schedule (void);

  private:
     /// Processor list, sorted by the time the processors become idle. 
    PHeap proc_list;
    PPrioItem *proc_item;
//    sched_time CP;

     /// Ready task, sorted by b_level.
    PHeap ready_task_by_blevel;
    PPrioItem *ready_task_by_blevel_item;
     /// Ready task, sorted by t_level.
    PHeap global_ready_task;
     /// Ready task, sorted by min_ST.
    PHeap *proc_ready_task;

    PFLB_f_Node ScheduleTask (void);
    void UpdateReadyTaskLists (PFLB_f_Node task);
};

#endif
