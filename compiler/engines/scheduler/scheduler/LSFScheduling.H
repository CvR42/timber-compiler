/* $Id: LSFScheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSFSCHEDULING_H
#define __LSFSCHEDULING_H

#include "../data/Heap.H"
#include "LSANode.H"
#include "Scheduling.H"


/**
  LSF = List Scheduling Algorithm  (by finish time)

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(V) + EP)
    
  Algorithm description
    - LSF schedules tasks in the order of their priority.
    - Each task is scheduled to the processor that allows its
      earliest finish time.

  Implementation details
    - The priority list is implemented as a heap.
*/
class LSFScheduling :
  public Scheduling
{
  public:
    LSFScheduling (void);
    ~LSFScheduling ();

     /// Schedule all the tasks in the DAG according to the current algorithm.
    void Schedule (void);

  private:
     /** Priotity task list */
    PHeap ready_task_list;


     /** Find the processor where this node can start the earliest.
         Schedule the task to the found processor. */
    void ScheduleTask (PLSANode node);

     /** Add the new ready tasks to the priority list */
    void UpdateReadyTaskList (PLSANode node);
};

#endif
