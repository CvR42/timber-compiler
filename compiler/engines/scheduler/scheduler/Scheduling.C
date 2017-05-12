/* $Id: Scheduling.C,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#include <stdio.h>

#include "../dag/DAG.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../dag/PrioDAGNode.H"
#include "ScheduleDAG.H"
#include "Scheduling.H"


PScheduling sched;
PDAG dag;
PProcessors proc;
PProcessors old_proc;



Scheduling::Scheduling (int v_task_ordering_alg)
{
  double mean_proc_speed = proc->GetProcSpeedMean();

  for (int n = 0; n < dag->GetNrNodes(); n++)
  {
    ((PDAGNode) dag->GetNode(n))->SetMeanExecTime (mean_proc_speed);
  }

  task_ordering_alg = v_task_ordering_alg;
}


void
Scheduling::Schedule (void)
{
  OrderTasks();
}


void
Scheduling::OrderTasks (void)
{
  switch (task_ordering_alg)
  {
    case STE:
      OrderTasks_EST();
      break;
    case LEV:
      OrderTasks_Level();
      break;
    case RCP:
      OrderTasks_RCP();
      break;
    default:
      fprintf (stderr,
               "Wrong task ordering algorithm: %d\n", task_ordering_alg);
      exit (-1);
  }
}


   /** Order the mapped tasks.
       At each step schedule the free task that start the earliest. */
void 
Scheduling::OrderTasks_EST (void)
{
     // The priority list of ready tasks
  PHeap ready_tasks = new Heap();

     // Add ready tasks to the ready task list and
     // set the number of unscheduled predecessors for the orhers.
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PPrioDAGNode node = (PPrioDAGNode) dag->GetNode (i);
    node->ComputeLevel();

    if (node->GetNrInEdges() == 0)
    {
      node->SetOrderPrio (0);
      ready_tasks->Enqueue (node);
    }
    else
    {
      node->InitNrUnschedPred();
    }
  }

     // Find the start time of the node.
  while (ready_tasks->Head() != NULL)
  {
    PPrioDAGNode node = (PPrioDAGNode) ready_tasks->Dequeue();
    int pid = node->GetProcID();
    sched_time ST = node->GetOrderPrio();

    if (proc->GetProcReadyTime(pid) > ST)
      ST = proc->GetProcReadyTime (pid);

    proc->AddTask (pid, node, ST);
    node->SetOldStartTime (ST);

       // Update ready task list.
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PPrioDAGNode succ = (PPrioDAGNode) node->GetSuccNode (i);

      if (succ->DecNrUnschedPred() == 0)
      {
        sched_time ST = ComputeStartTime (succ);
        succ->SetOrderPrio (ST);
        ready_tasks->Enqueue (succ);
      }
    }
  }

  delete ready_tasks;
}


   /** Order the mapped tasks.
       At each step schedule the free task with the max (t_level + b_level). */
void
Scheduling::OrderTasks_Level (void)
{
     // The priority list of ready tasks
  PHeap ready_tasks = new Heap();

     // Add ready tasks to the ready task list and
     // set the number of unscheduled predecessors for the orhers.
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PPrioDAGNode node = (PPrioDAGNode) dag->GetNode (i);
    node->SetOrderPrio (NONE);
  }
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PPrioDAGNode node = (PPrioDAGNode) dag->GetNode (i);

    node->ComputeOrderPrio();
    node->ComputeLevel();

    if (node->GetNrInEdges() == 0)
      ready_tasks->Enqueue (node);
    else
      node->InitNrUnschedPred();
  }

     // Find the start time of the node.
  while (ready_tasks->Head() != NULL)
  {
    PPrioDAGNode node = (PPrioDAGNode) ready_tasks->Dequeue();
    sched_time ST = ComputeStartTime (node);   // based only on task dependencies
    int pid = node->GetProcID();

    if (proc->GetProcReadyTime(pid) > ST)
      ST = proc->GetProcReadyTime (pid);

    proc->AddTask (pid, node, ST);
    node->SetOldStartTime (ST);

       // Update ready task list.
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PPrioDAGNode succ = (PPrioDAGNode) node->GetSuccNode (i);

      if (succ->DecNrUnschedPred() == 0)
        ready_tasks->Enqueue (succ);
    }
  }

  delete ready_tasks;
}  


void
Scheduling::OrderTasks_RCP (void)
{
  PHeap ready_tasks = new Heap;

     // Add ready tasks to the ready task list and
     // set the number of unscheduled predecessors for the orhers.
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PPrioDAGNode node = (PPrioDAGNode) dag->GetNode (i);

    node->ComputeOrderPrio();
    node->ComputeLevel();

    if (node->GetNrInEdges() == 0)
    {
      node->SetOldStartTime (0);
      ready_tasks->Enqueue (node);
    }
    else
    {
      node->InitNrUnschedPred();
    }
  }

  while (ready_tasks->Head() != NULL)
  {
    PPrioDAGNode node = (PPrioDAGNode) ready_tasks->Dequeue();
    int pid = node->GetProcID();
    sched_time ST = node->ComputeStartTimeOnProc (pid);
    proc->AddTask (pid, node, ST);

       // Update ready task list
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PPrioDAGNode succ = (PPrioDAGNode) node->GetSuccNode (i);

      if (succ->DecNrUnschedPred() == 0)
        ready_tasks->Enqueue (succ);
    }
  }

  delete ready_tasks;
}


void
Scheduling::UpdateProc (int pid, PHeap *ready_tasks_on_proc,
                           PHeap proc_list, PPrioItem *proc_item)
{
  if (ready_tasks_on_proc[pid]->Head() != NULL)
  {
    PPrioDAGNode node = (PPrioDAGNode) ready_tasks_on_proc[pid]->Head();
    sched_time first_EST = node->GetOldStartTime();
    PProcListItem nl = proc->GetLastTask(pid);

    if (nl != NULL)
    {
      sched_time available = nl->GetFinishTime();

      proc_item[pid]->SetPrio (-max(available, first_EST));
    }
    else
    {
      proc_item[pid]->SetPrio (-first_EST);
    }
  }
  else
  {
    proc_item[pid]->SetPrio (MIN_TIME);
  }

  proc_list->BalanceHeap (proc_item[pid]);
}


   //
   // Compute the start time of a task based only on the task dependencies
   //
sched_time
Scheduling::ComputeStartTime (PPrioDAGNode node)
{
  sched_time start_time = 0;

  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge edge = (PDAGEdge) node->GetInEdge (i);
    PPrioDAGNode pred = (PPrioDAGNode) edge->GetFromNode();
    int pred_pid = pred->GetProcID();
    sched_time tmp = pred->GetOldStartTime() + pred->GetExecTime (pred_pid);
    tmp += edge->GetCommTime (node->GetProcID(), pred->GetProcID());

    if (tmp > start_time)
      start_time = tmp;
  }

  return start_time;
}
