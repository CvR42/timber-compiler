/* $Id: FFF-p_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __FFF_P_SCHEDULING_H
#define __FFF_P_SCHEDULING_H

#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../data/PseudoHeap.H"
#include "../data/PseudoPrioItem.H"
#include "Scheduling.H"
#include "FLB-p_Node.H"


class FFF_p_Scheduling
  : public Scheduling
{
  public:
    FFF_p_Scheduling (void);
    ~FFF_p_Scheduling ();

    void Schedule (void);

  private:
     /// Processor list, sorted by the time the processors become idle. 
    PHeap proc_list;
    PPrioItem *proc_item;
//    sched_time CP;

     /// Ready task, sorted by b_level.
    PPseudoHeap ready_task_by_blevel;
    PPseudoPrioItem *ready_task_by_blevel_item;
     /// Ready task, sorted by t_level.
    PPseudoHeap global_ready_task;
     /// Ready task, sorted by min_ST.
    PPseudoHeap *proc_ready_task;

    PFLB_p_Node ScheduleTask (void);
    void UpdateReadyTaskLists (PFLB_p_Node task);
};

#endif
