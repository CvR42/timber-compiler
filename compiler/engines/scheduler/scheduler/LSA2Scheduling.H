/* $Id: LSA2Scheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __LSA2SCHEDULING_H
#define __LSA2SCHEDULING_H

#include "../data/PrioItem.H"
#include "../data/Heap.H"
#include "LSA2Node.H"
#include "Scheduling.H"


/**
  LSA2
  
  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(V) + E)
    
  Algorithm description
    Same like LSA, but processor selection is performed
    trying only (a) the processor last message arrives from and
                (b) the processor becoming idle the earliest.
*/
class LSA2Scheduling :
  public Scheduling
{
  public:
    LSA2Scheduling (void);
    ~LSA2Scheduling ();

     /// Schedule all the tasks in the DAG according to the current algorithm.
    void Schedule (void);

  private:
     /// Processor list, sorted by the time the processors become idle. 
    PHeap proc_list;
    PPrioItem *proc_item;

     /// Priotity task list
    PHeap ready_task_list;


     /** Find the processor where this node can start the earliest.
         Schedule the task to the found processor. */
    void ScheduleTask (PLSA2Node node);

     /** Add the new ready tasks to the priority list */
    void UpdateReadyTaskList (PLSA2Node node);
};

#endif
