/* $Id: FCP-p_Scheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __FCP_p_SCHEDULING_H
#define __FC_p_1SCHEDULING_H

#include "../data/PseudoHeap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "FCP-p_Node.H"


/**
  FCP-p (Fast Critical Path)  by  Andrei - Nov 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(P) + E)
    
  Algorithm description
    1.   Compute bottom levels (b_level) for all tasks.
    2.   Repeat until all tasks are scheduled.
    2.1.   Find the processor to which the next node will be mapped,
             as the processor becoming idle the earliest.
    2.2.   Find the task with the minimum b_level.
             The first X readys are sorted by their b_level,
             the other ready tasks are kept in a FIFO list
    2.3.   Schedule the chosen task to the processor.
    2.4.   Add the new tasks (because of the current task scheduling) to
             the ready task lists.
*/
class FCP_p_Scheduling
  : public Scheduling
{
  public:
     /** Build the task and processor priority lists. */
    FCP_p_Scheduling (double pseudo_heap_size);
    ~FCP_p_Scheduling ();

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
    PPseudoHeap ready_task;

    double mean_flops;

     /** 2.2. Find the task with the minimum b_level.
                The first X readys are sorted by their b_level,
                the other ready tasks are kept in a FIFO list
         2.3. Schedule the chosen task to the processor. */
    PFCP_p_Node ScheduleTask (void);

     /** 2.4. Add the new tasks (because of the current task scheduling) to
              the proper task lists. */
    void UpdateReadyTaskList (PFCP_p_Node node);

    void AddReadyTask (PFCP_p_Node node);
    PFCP_p_Node GetReadyTask (void);
};

#endif
