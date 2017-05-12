/* $Id: LSAScheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSASCHEDULING_H
#define __LSASCHEDULING_H

#include "../data/Heap.H"
#include "LSANode.H"
#include "Scheduling.H"


/**
  LSA = List Scheduling Algorithm

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(V) + EP)
    
  Algorithm description
    - LSA schedules tasks in the order of their priority.
    - Each task is scheduled to the processor that allows its
      earliest starting time.

  Implementation details
    - The priority list is implemented as a heap.
*/
class LSAScheduling :
  public Scheduling
{
  public:
    LSAScheduling (void);
    ~LSAScheduling ();

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
