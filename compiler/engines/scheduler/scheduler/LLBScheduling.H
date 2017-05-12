/* $Id: LLBScheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __LLBSCHEDULING_H
#define __LLBSCHEDULING_H

#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "LLBNode.H"


/**
  Cluster Merging to suits the number of available processors
  by  Radulescu, van Gemund, Lin - 98

  Characteristics
    - O ()
    
  Algorithm description
    1.   Compute b_level as the priority for each task.
    2.   Repeat until all tasks are scheduled.
    2.1.   Find the processor to which the next node will be mapped,
             as the processor becoming idle the earliest.
    2.2.   Find the most prioritar mapped task.
    2.3.   Find the most prioritar unmapped task.
    2.4.   Between the above two tasks, choose the one that start the earliest.
 ?  2.5.   If no such task exists, choose the most prioritar task mapped on
             one of the other processors.
    2.6.   Schedule the chosen task to the processor.
    2.7.   If the task was not already mapped, map also its own cluster.
    2.8.   Add the new tasks (because of the current task scheduling) to
             the proper task lists.
*/
class LLBScheduling
  : public Scheduling
{
  public:
     /** Build the task and processor priority lists. */
    LLBScheduling (void);

     /**
        While there are unscheduled tasks, pick the first processor
        that become idle and map a task to it.
     */
    void Schedule (void);

  private:
     /// Processor list, sorted by the time the processors become idle. 
    PHeap proc_list;
    PPrioItem *proc_item;

     /// Ready task lists, sorted by b_level.
    PHeap all_ready_unmapped_tasks;
    PHeap *ready_mapped_tasks;
    PHeap all_ready_mapped_tasks;

     /// Items for the task lists.
    PPrioItem *all_ready_unmapped_task_item;
    PPrioItem *all_ready_mapped_task_item;
    PPrioItem *ready_mapped_task_item;

     /** 2.2. Find the most prioritar mapped task.
         2.3. Find the most prioritar unmapped task.
         2.4. Between the above 2 tasks, choose the one starting the earliest.
      ?  2.5. If no such task exists, choose the most prioritar task mapped on 
              one of the other processors.
         2.6. Schedule the chosen task to the processor.
         2.7. If the task was not already mapped, map also its own cluster. */
    PLLBNode ScheduleTaskOnProc (int pid);

     /** 2.8. Add the new tasks (because of the current task scheduling) to
              the proper task lists. */
    void UpdateTaskLists (PLLBNode node);

     /** Map all the tasks belonging to a cluster to a processor. */
    void MapTaskCluster (int cluster, int pid); 
};

#endif
