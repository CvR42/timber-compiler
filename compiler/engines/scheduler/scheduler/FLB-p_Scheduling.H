/* $Id: FLB-p_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __FLB_p_SCHEDULING_H
#define __FLB_p_SCHEDULING_H

#include "../data/Heap.H"
#include "../data/PseudoHeap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "FLB-p_Node.H"


/**
  FLB-p  by  Andrei - Jan 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O (V log(P) + E)
    
  Algorithm description
    The same as FLB-f, but the priority lists are real heaps, not pseudo-heaps.
*/
class FLB_p_Scheduling
  : public Scheduling
{
  public:
     /** Build the task and processor priority lists. */
    FLB_p_Scheduling (double pseudo_heap_size);
    ~FLB_p_Scheduling ();

     /**
        While there are unscheduled tasks, schedule one.
     */
    void Schedule (void);

  private:
     /// Processor lists, sorted by the time the processors become idle. 
    PHeap active_proc_list;
    PHeap all_proc_list;
    PPrioItem *active_proc_item;
    PPrioItem *all_proc_item;

     /// Ready task, sorted by t_level.
    PPseudoHeap global_ready_task;
     /// Ready task, sorted by min_ST.  Contains tasks in ready_task_heap.
    PPseudoHeap *proc_ready_task;

    double mean_flops;


     /**     2.1.   IF global_list not empty
             2.1.1.   pid := processor becoming idle the earliest
             2.2.   ELSE
             2.2.1.   pid := active processor becomming idle the earliest
             2.3.   task1 := dequeue (proc_list[pid])
             2.4.   task2 := dequeue (global_list)
             2.5.   IF ST (task1,pid) < ST (task2,pid)
             2.5.1.   task := task1
             2.6.   ELSE
             2.6.1.   task := task2
             2.7.   Schedule the task on pid
             2.8.   Move the tasks with t_level > PIT(pid) to global task list
             2.9.   Update the ready task lists.
             2.10.  Add the new tasks (because of the current task scheduling)
                      to the ready task lists. */
    PFLB_p_Node ScheduleTask (void);
};

#endif
