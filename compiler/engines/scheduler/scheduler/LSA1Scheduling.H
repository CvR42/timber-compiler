/* $Id: LSA1Scheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSA1SCHEDULING_H
#define __LSA1SCHEDULING_H

#include "../data/PseudoHeap.H"
#include "LSA1Node.H"
#include "Scheduling.H"


/**
  LSA1 = List Scheduling Algorithm

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(V) + EP)
    
  Algorithm description
    - LSA1 schedules tasks in the order of their priority.
    - Each task is scheduled to the processor that allows its
      earliest starting time.

  Implementation details
    - The priority list is implemented as a fixed size heap
      combined with a FIFO list
*/
class LSA1Scheduling :
  public Scheduling
{
  public:
    LSA1Scheduling (double heap_size);
    ~LSA1Scheduling ();

     /// Schedule all the tasks in the DAG according to the current algorithm.
    void Schedule (void);

  private:
     /** Priotity task list */
    PPseudoHeap ready_task_list;


     /** Find the processor where this node can start the earliest.
         Schedule the task to the found processor. */
    void ScheduleTask (PLSA1Node node);

     /** Add the new ready tasks to the priority list */
    void UpdateReadyTaskList (PLSA1Node node);
};

#endif
