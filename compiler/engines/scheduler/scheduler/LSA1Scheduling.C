/* $Id: LSA1Scheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

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

#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/PseudoHeap.H"
#include "../processors/Processors.H"
#include "LSA1Node.H"
#include "Scheduling.H"
#include "LSA1Scheduling.H"


LSA1Scheduling::LSA1Scheduling (double heap_size)
{
  ready_task_list = new PseudoHeap (heap_size * proc->GetNrProc());

  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PLSA1Node task = (PLSA1Node) dag->GetNode (nid);
    task->InitNrUnschedPred();

    if (task->GetNrInEdges() == 0)
    {
      task->ComputePrio();
      ready_task_list->Enqueue (task);
    }
  }
}

LSA1Scheduling::~LSA1Scheduling ()
{
  delete ready_task_list;
}


   /** Schedule all the tasks in the DAG according to the current algorithm. */
void
LSA1Scheduling::Schedule (void)
{
  for (PLSA1Node task = (PLSA1Node) ready_task_list->Dequeue();
       task;  task = (PLSA1Node) ready_task_list->Dequeue())
  {
    ScheduleTask (task);
    UpdateReadyTaskList (task);
  }
}


   /** Find the processor where this task can start the earliest.
       Schedule the task to the found processor. */
void
LSA1Scheduling::ScheduleTask (PLSA1Node task)
{
  sched_time min_start_time = NONE;
  int pid = 0;

     // Find the processor where this task can start the earliest.
  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    sched_time t = task->ComputeStartTimeOnProc (p);

    if ((min_start_time == NONE) || (t < min_start_time))
    {
      min_start_time = t;
      pid = p;
    }
  }

     // Add this task after the previous scheduled task on 'pid'.
     // Set the start time of this task on 'pid'.
  proc->AddTask (pid, task, min_start_time);
}


   /** Add the new tasks (because of the current task scheduling) to
       the proper task lists. */
void
LSA1Scheduling::UpdateReadyTaskList (PLSA1Node task)
{
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PLSA1Node succ = (PLSA1Node) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      task->ComputePrio();
      ready_task_list->Enqueue (succ);
    }
  }
}
