/* $Id: ETFScheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __ETFSCHEDULING_H
#define __ETFSCHEDULING_H

#include "../data/List.H"
#include "Scheduling.H"
#include "ETFNode.H"


/**
  ETF (Earliest Task First)  by  Hwang, Chow, Anger, Lee - Apr 89

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V^2 P)

  Algorithm description
    - The ETF algorithm uses a set called _ready task set_. A task is said 
      to be _ready_ when all its predecessors are scheduled.
    - The algorithm calculates the earliest start time of every task that
      belongs to the ready task list on every idle procesor. The tasks are
      assigned in the ascending order of their earliest start times.
    - The set of ready tasks and as each task finishes its
      execution.

  Implementation:
    - The set of ready tasks is a usual list.  It is not necessarily
      to sort the ready tasks.
*/
class ETFScheduling
  : public Scheduling
{
  public:
     /** Build the ready task list. */
    ETFScheduling (void);
    ~ETFScheduling ();

    void Schedule (void);

  private:
     /// Ready task list.
    PList ready_tasks_list;

     /** The algorithm calculates the earliest start time of every task
         that belongs to the ready task list on every idle procesor.
         The tasks are assigned in the ascending order of their
         earliest start times. */
    void ScheduleTask (void);

     /** Add the new tasks (because of the current task scheduling) to
         the proper task lists. */
    void UpdateReadyTaskList (PETFNode node);

    void AddReadyTask (PETFNode node);
    PETFNode RemoveReadyTask (void);
};

#endif
