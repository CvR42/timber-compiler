/* $Id: FLB-p_Scheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

/**
  FLB-p  by  Andrei - Jan 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(P) + E)
    
  Algorithm description
    The same as FLB-f, but the priority lists are real heaps, not pseudo-heaps.
*/
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "../data/PseudoHeap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "FLB-p_Node.H"
#include "Scheduling.H"
#include "FLB-p_Scheduling.H"


   /**
      Build the task and processor priority lists.
   */
FLB_p_Scheduling::FLB_p_Scheduling (double pseudo_heap_size)
    : Scheduling()
{
  mean_flops = 0;
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PDAGNode node = dag->GetNode (nid);
    mean_flops += node->GetFlops();
  }
  mean_flops /= dag->GetNrNodes();

  active_proc_list = new Heap();
  all_proc_list = new Heap();
  active_proc_item = new PPrioItem [proc->GetNrProc()];
  all_proc_item = new PPrioItem [proc->GetNrProc()];
  proc_ready_task = new PPseudoHeap [proc->GetNrProc()];
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    double prio = - mean_flops / proc->GetProcSpeed (pid);
    active_proc_item[pid] = new PrioItem (pid, 0, proc->GetProcSpeed(pid));
    all_proc_item[pid] = new PrioItem (pid, prio, proc->GetProcSpeed(pid));
    all_proc_list->Enqueue (all_proc_item[pid]);
    proc_ready_task[pid] = new PseudoHeap (pseudo_heap_size * proc->GetNrProc());
  }

  global_ready_task = new PseudoHeap (pseudo_heap_size * proc->GetNrProc());
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PFLB_p_Node node = (PFLB_p_Node) dag->GetNode (nid);
    node->ComputeBLevel();
    node->InitNrUnschedPred();

    if (node->GetNrInEdges() == 0)
      global_ready_task->Enqueue (node);
  }
}

FLB_p_Scheduling::~FLB_p_Scheduling ()
{
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    delete active_proc_item[pid];
    delete all_proc_item[pid];
    delete proc_ready_task[pid];
  }
  delete active_proc_item;
  delete all_proc_item;
  delete active_proc_list;
  delete all_proc_list;
  delete proc_ready_task;
  delete global_ready_task;
}


   /**
      While there are unscheduled tasks, schedule one.
   */
void
FLB_p_Scheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
  {
    ScheduleTask();
  }
}


   /**
      2.1.   IF global_task_list not empty
      2.1.1.   pid := processor becoming idle the earliest
      2.2.   ELSE
      2.2.1.   pid := active processor becomming idle the earliest
      2.3.   task1 := dequeue (proc_task_list[pid])
      2.4.   task2 := dequeue (global_task_list)
      2.5.   IF ST (task1,pid) < ST (task2,pid)
      2.5.1.   task := task1
      2.6.   ELSE
      2.6.1.   task := task2
      2.7.   Schedule the task on pid
      2.8.   Move the tasks with t_level > PIT(pid) to global task list
      2.9.   Update the ready task lists.
      2.10.  Add the new tasks (because of the current task scheduling) to
               the ready task lists.
   */
PFLB_p_Node
FLB_p_Scheduling::ScheduleTask (void)
{
  int pid, pid_all = NONE, pid_active = NONE;
  PFLB_p_Node task;

  if (DEBUG)
  {
    puts ("\nGlobal: ");
    global_ready_task->Print();
    for (int p = 0; p < proc->GetNrProc(); p++)
    {
      printf ("\nProc %d: ", p);
      proc_ready_task[p]->Print();
    }
    putchar ('\n');
  }

     // 2.1.   pid_active := active processor with minimum min_ST
     // 2.2.   pid_all := processor becomming idle the earliest
  PPrioItem prio_item;
  prio_item = (PPrioItem) active_proc_list->Head();
  if (prio_item != NULL)
    pid_active = prio_item->GetIDX();
  prio_item = (PPrioItem) all_proc_list->Head();
  if (prio_item != NULL)
    pid_all = prio_item->GetIDX();

     // 2.3.   task1 := head (proc_task_list[pid])
     // 2.4.   task2 := head (global_task_list)
     // 2.5.   IF ST (task1,pid_active) < ST (task2,pid_all)
     // 2.5.1.   task := task1
     // 2.6.   ELSE
     // 2.6.1.   task := task2
  PFLB_p_Node task1 = NULL, task2 = NULL;
  if (pid_active != NONE)
    task1 = (PFLB_p_Node) proc_ready_task[pid_active]->Head();
  task2 = (PFLB_p_Node) global_ready_task->Head();
  sched_time ST1 = MAXDOUBLE, ST2 = MAXDOUBLE;
  sched_time FT1 = MAXDOUBLE, FT2 = MAXDOUBLE;
  if (task1 != NULL)
  {
    ST1 = task1->ComputeStartTimeOnProc (pid_active);
    FT1 = ST1 + task1->GetExecTime (pid_active);
  } 
  if (task2 != NULL)
  {
    ST2 = task2->ComputeStartTimeOnProc (pid_all);
    FT2 = ST2 + task2->GetExecTime (pid_all);
  } 

  if (FT1 < FT2)
  {
    task = task1;
    pid = pid_active;
    proc_ready_task[pid]->Dequeue();
    active_proc_list->Dequeue();
  }
  else
  {
    task = task2;
    pid = pid_all;
    global_ready_task->Dequeue();
  }

     // 2.7.   Schedule the task on pid
  sched_time ST = task->ComputeStartTimeOnProc (pid);
  proc->AddTask (pid, task, ST);
  if (DEBUG)
  {
    printf ("------------------\n");
    for (int i = 0; i < pid; i++)
      printf ("              ");
    printf ("%d[%3.0f-%3.0f]\n", task->GetID(), ST, ST + task->GetExecTime(pid));
    printf ("------------------\n");

//    proc->Display();
    fflush (stdout);
  }

     // 2.8.   Move the tasks with t_level > PIT(pid) to global task list
  sched_time proc_idle_time = proc->GetProcReadyTime (pid);
  for (;;)
  {
    PFLB_p_Node t = (PFLB_p_Node) proc_ready_task[pid]->Head();
    if (t == NULL)
    {
      active_proc_list->RemoveItem (active_proc_item[pid]);
      break;
    }
    if (t->GetTLevel() >= proc_idle_time)
    {
      active_proc_item[pid]->SetPrio (-t->GetMinST() - t->GetExecTime(pid));
      active_proc_list->BalanceHeap (active_proc_item[pid]);
      break;
    }

    proc_ready_task[pid]->Dequeue();
    t->SetProcID (NONE);
    global_ready_task->Enqueue (t);
  }

     // 2.9.  Update the processor lists.
  PPrioItem item;
  PFLB_p_Node t = (PFLB_p_Node) proc_ready_task[pid]->Head();
  if (t != NULL)
  {
    item = active_proc_item[pid];
    sched_time prio_time = t->GetMinST() + t->GetExecTime (pid);
    if (prio_time < proc_idle_time)
      prio_time = proc_idle_time;
    item->SetPrio (-prio_time);
    if (item->GetIndexInHeap() == NONE)
      active_proc_list->Enqueue (item);
    else
      active_proc_list->BalanceHeap (item);
  }

  item = all_proc_item[pid];
  item->SetPrio (-proc_idle_time - mean_flops / proc->GetProcSpeed (pid));
  all_proc_list->BalanceHeap (item);

     // 2.10.   Add the new tasks (because of the current task scheduling) to
     //         the ready task lists.
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PFLB_p_Node succ = (PFLB_p_Node) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      succ->ComputeReadyVals();
      int p = succ->GetProcID();

      if (succ->GetTLevel() >= proc->GetProcReadyTime (p))
      {
        if (proc_ready_task[p]->Head() == NULL)
        {
          if (succ->GetMinST() >= proc->GetProcReadyTime (p))
            active_proc_item[p]->SetPrio (-succ->GetMinST() - succ->GetExecTime(p));
          else
            active_proc_item[p]->SetPrio (-proc->GetProcReadyTime (p) - succ->GetExecTime(p));
          active_proc_list->Enqueue (active_proc_item[p]);
        }
        proc_ready_task[p]->Enqueue (succ);
      }
      else
      {
        succ->SetProcID (NONE);
        global_ready_task->Enqueue (succ);
      }
    }
  }

  return task;
}
