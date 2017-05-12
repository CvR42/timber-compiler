/* $Id: FCP-f_Scheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __FCP_f_SCHEDULING_H
#define __FCP_f_SCHEDULING_H

#include <queue>
#include "../al/mapint.h"

#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "FCP-f_Node.H"


/**
  FCP-f (Fast Critical Path)  by  Andrei - Nov 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(W) + E)
    
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

class FCP_f_comp
{
  public:
    bool operator() (const PFCP_f_Node &a, const PFCP_f_Node &b)
    {
      return a->LessPrioThan (b);
    }
};


class FCP_f_Scheduling
  : public Scheduling
{
  public:
     /** Build the task and processor priority lists. */
    FCP_f_Scheduling (void);
    ~FCP_f_Scheduling ();

     /**
        While there are unscheduled tasks, pick the first processor
        that become idle and map a task to it.
     */
    void Schedule (void);

  private:
     /// Processor list, sorted by the time the processors become idle. 
    mapint<sched_time> *proc_list;

     /// Ready task lists, sorted by b_level.
    priority_queue <PFCP_f_Node, deque<PFCP_f_Node>, FCP_f_comp> ready_task;

    double mean_flops;

     /** 2.2. Find the task with the minimum b_level.
                The first X readys are sorted by their b_level,
                the other ready tasks are kept in a FIFO list
         2.3. Schedule the chosen task to the processor. */
    PFCP_f_Node ScheduleTask (void);

     /** 2.4. Add the new tasks (because of the current task scheduling) to
              the proper task lists. */
    void UpdateReadyTaskList (PFCP_f_Node node);

    void AddReadyTask (PFCP_f_Node node);
    PFCP_f_Node GetReadyTask (void);
};

#endif
