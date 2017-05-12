/* $Id: LSFScheduling.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

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

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "LSANode.H"
#include "Scheduling.H"
#include "LSFScheduling.H"


LSFScheduling::LSFScheduling (void)
{
  ready_task_list = new Heap();

  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PLSANode task = (PLSANode) dag->GetNode (nid);
    task->InitNrUnschedPred();

    if (task->GetNrInEdges() == 0)
    {
      task->ComputePrio();
      ready_task_list->Enqueue (task);
    }
  }
}

LSFScheduling::~LSFScheduling ()
{
  delete ready_task_list;
}


   /** Schedule all the tasks in the DAG according to the current algorithm. */
void
LSFScheduling::Schedule (void)
{
  for (PLSANode task = (PLSANode) ready_task_list->Dequeue();
       task;  task = (PLSANode) ready_task_list->Dequeue())
  {
    ScheduleTask (task);
    UpdateReadyTaskList (task);
  }
}


   /** Find the processor where this task can start the earliest.
       Schedule the task to the found processor. */
void
LSFScheduling::ScheduleTask (PLSANode task)
{
  sched_time min_start_time = NONE;
  sched_time min_finish_time = NONE;
  int pid = 0;

     // Find the processor where this task can start the earliest.
  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    sched_time st = task->ComputeStartTimeOnProc (p);
    sched_time t = st + task->GetExecTime (p);

    if ((min_finish_time == NONE) || (t < min_finish_time))
    {
      min_start_time = st;
      min_finish_time = t;
      pid = p;
    }
  }

     // Add this task after the previous scheduled task on 'pid'.
     // Set the start time of this task on 'pid'.
  proc->AddTask (pid, task, min_start_time);

  if (DEBUG)
  {
    printf ("------------------\n");
    for (int i = 0; i < pid; i++)
      printf ("              ");
    printf ("%d[%3.0f-%3.0f]\n", task->GetID(),
                                 task->GetStartTime(), task->GetFinishTime());
  }
}


   /** Add the new tasks (because of the current task scheduling) to
       the proper task lists. */
void
LSFScheduling::UpdateReadyTaskList (PLSANode task)
{
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PLSANode succ = (PLSANode) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      succ->ComputePrio();
      ready_task_list->Enqueue (succ);
    }
  }
}
