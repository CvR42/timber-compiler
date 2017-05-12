/* $Id: FLB-f_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __FLB_f_SCHEDULING_H
#define __FLB_f_SCHEDULING_H

#include "../data/Heap.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "FLB-f_Node.H"


/**
  FLB-f  by  Andrei - Jan 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O (V log(W) + E)
    
  Algorithm description
    Task
    ----
    For each ready tasks t the following values are used:
      t_level = minimum message arrival time (no comm cost zeroing)
      P(t) = the processor from where the last message arrives.
      min_ST = minimum start time, obtained by zeroing 
               the communication costs from the P(t)
               from which the last message arrives.

    - Communication can be reduced only by scheduling the task to the processor
      from which the last message arrives.
    - If the processor is busy at that time, the starting time is
      higher than t_level and is determined by the processor idle time.

    Task management:
    - When the task becomes ready, assign it to the processor the last msg
      arrives from.  The tasks assigned to a procesor are sorted by their
      start time on that processor.
    - When the task's processor becomes busy at the time of the last msg
      arrival, move the task to a global list sorted by t_level.
      The task start time is t_level.

    Processor
    ---------
    - active, if it has tasks assigned with it
    - passive, otherwise

    Processor management:
    - If global_list not empty
        pid = processor becoming idle the earliest
    - Else
        pid = active processor becomming idle the earliest

    Scheduling
    ----------
    task1 := dequeue (proc_list[pid])
    task2 := dequeue (global_list)
      - at least one of these task exists.
    => Schedule the task starting the earliest to pid.
    
    1.   Build the ready task lists:
           - All proc_list are empty
           - global_list stores entry tasks
    2.   REPEAT until all tasks are scheduled.
    2.1.   IF global_list not empty
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
    2.10.  Add the new tasks (because of the current task scheduling) to
             the ready task lists.
*/
class FLB_f_Scheduling
  : public Scheduling
{
  public:
     /** Build the task and processor priority lists. */
    FLB_f_Scheduling (void);
    ~FLB_f_Scheduling ();

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
    PHeap global_ready_task;
     /// Ready task, sorted by min_ST.  Contains tasks in ready_task_heap.
    PHeap *proc_ready_task;

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
    PFLB_f_Node ScheduleTask (void);
};

#endif
