/* $Id: LSA2Scheduling.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

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

#include <stdlib.h>
#include <stdio.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "LSA2Node.H"
#include "Scheduling.H"
#include "LSA2Scheduling.H"


LSA2Scheduling::LSA2Scheduling (void)
{
  ready_task_list = new Heap();
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PLSA2Node task = (PLSA2Node) dag->GetNode (nid);
    task->InitNrUnschedPred();

    if (task->GetNrInEdges() == 0)
    {
      task->ComputePrio();
      ready_task_list->Enqueue (task);
    }
  }

  proc_list = new Heap();
  proc_item = new PPrioItem [proc->GetNrProc()];
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    proc_item[pid] = new PrioItem (pid, 0);
    proc_list->Enqueue (proc_item[pid]);
  }
}

LSA2Scheduling::~LSA2Scheduling ()
{
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    delete proc_item[pid];
  }
  delete proc_item;
  delete proc_list;

  delete ready_task_list;
}


   /** Schedule all the tasks in the DAG according to the current algorithm. */
void
LSA2Scheduling::Schedule (void)
{
  for (PLSA2Node task = (PLSA2Node) ready_task_list->Dequeue();
       task;  task = (PLSA2Node) ready_task_list->Dequeue())
  {
    ScheduleTask (task);
    UpdateReadyTaskList (task);
  }
}


   /** Find the processor where this task can start the earliest.
       Schedule the task to the found processor. */
void
LSA2Scheduling::ScheduleTask (PLSA2Node task)
{
     // Find the processor where this task can start the earliest.
  int p1 = task->GetProcID();
  int p2 = ((PPrioItem) proc_list->Head())->GetIDX();
  
  sched_time t1 = task->ComputeStartTimeOnProc (p1);
  sched_time t2 = task->ComputeStartTimeOnProc (p2);

  if (t1 < t2)
  {
    proc->AddTask (p1, task, t1);
    proc_item[p1]->SetPrio (-task->GetFinishTime());
    proc_list->BalanceHeap (proc_item[p1]);
  }
  else
  {
    proc_list->Head();
    proc->AddTask (p2, task, t2);
    proc_item[p2]->SetPrio (-task->GetFinishTime());
    proc_list->BalanceHeap (proc_item[p2]);
  }

  if (DEBUG)
  {
    printf ("------------------\n");
    for (int i = 0; i < task->GetProcID(); i++)
      printf ("              ");
    printf ("%d[%3.0f-%3.0f]\n", task->GetID(),
                                 task->GetStartTime(), task->GetFinishTime());
  }
}


   /** Add the new tasks (because of the current task scheduling) to
       the proper task lists. */
void
LSA2Scheduling::UpdateReadyTaskList (PLSA2Node task)
{
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PLSA2Node succ = (PLSA2Node) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      succ->ComputePrio();
      succ->SetProcLastMsg();
      ready_task_list->Enqueue (succ);
    }
  }
}
