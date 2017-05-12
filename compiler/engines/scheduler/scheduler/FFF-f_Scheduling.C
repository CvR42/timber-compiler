/* $Id: FFF-f_Scheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "FLB-f_Node.H"
#include "Scheduling.H"
#include "FFF-f_Scheduling.H"


   /**
      Build the task and processor priority lists.
   */
FFF_f_Scheduling::FFF_f_Scheduling (void)
    : Scheduling()
{
  proc_list = new Heap (proc->GetNrProc());
  proc_item = new PPrioItem [proc->GetNrProc()];
  proc_ready_task = new PHeap [proc->GetNrProc()];
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    proc_ready_task[pid] = new Heap();
    proc_item[pid] = new PrioItem (pid, 0);
    proc_list->Enqueue (proc_item[pid]);
  }

  global_ready_task = new Heap();
  ready_task_by_blevel = new Heap();
  ready_task_by_blevel_item = new PPrioItem [dag->GetNrNodes()];
//  CP = 0;
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PFLB_f_Node node = (PFLB_f_Node) dag->GetNode (nid);

    node->ComputeBLevel();
//    if (node->GetBLevel() > CP)
//      CP = node->GetBLevel();

    node->InitNrUnschedPred();
    if (node->GetNrInEdges() == 0)
    {
      global_ready_task->Enqueue (node);
      ready_task_by_blevel_item[nid] = new PrioItem (nid, node->GetBLevel());
      ready_task_by_blevel->Enqueue (ready_task_by_blevel_item[nid]);
    }
  }
}

FFF_f_Scheduling::~FFF_f_Scheduling ()
{
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
    delete proc_ready_task[pid];

  delete proc_list;
  delete proc_item;
  delete proc_ready_task;
  delete global_ready_task;
  delete ready_task_by_blevel;
  delete ready_task_by_blevel_item;
}


   /**
      While there are unscheduled tasks, schedule one.
   */
void
FFF_f_Scheduling::Schedule (void)
{
  while (ready_task_by_blevel->Head() != 0)
  {
    ScheduleTask();
  }
}


PFLB_f_Node
FFF_f_Scheduling::ScheduleTask (void)
{
     // 2.1.   task := head (ready_task_by_blevel)
     // 2.2.   pid := enabling processor (if EP type), or
     //               processor becomming idle the earliest (if non EP type)
  PPrioItem task_item = (PPrioItem) ready_task_by_blevel->Head();
  PFLB_f_Node task = (PFLB_f_Node) dag->GetNode (task_item->GetIDX());
  int pid = task->GetProcID();
  if (pid == NONE)
    pid = ((PPrioItem) proc_list->Head())->GetIDX();

     // 2.3.   task1 := head (proc_task_list[pid])
     // 2.4.   task2 := head (global_task_list)
  PFLB_f_Node task1 = (PFLB_f_Node) proc_ready_task[pid]->Head();
  if (task1 == task)
    task1 = 0;
  PFLB_f_Node task2 = (PFLB_f_Node) global_ready_task->Head();
  if (task2 == task)
    task2 = 0;

     // 2.5. Find the task with the minimum start time on 'pid'.
  sched_time ST = task->ComputeStartTimeOnProc (pid);
  sched_time ST1 = MAXDOUBLE, ST2 = MAXDOUBLE;
  if (task1 != NULL)
    ST1 = task1->ComputeStartTimeOnProc (pid);
  if (task2 != NULL)
    ST2 = task2->ComputeStartTimeOnProc (pid);

  if (proc_list->GetSize() > 1)
    if ((ST1 < ST) && (ST1 <= ST2))
    {
      task = task1;
      ST = ST1;
    }
    else if (ST2 < ST)
    if (ST2 < ST)
    {
      task = task2;
      ST = ST2;
    }

  if (task->GetProcID() != NONE)
    proc_ready_task[pid]->RemoveItem (task);
  else
    global_ready_task->RemoveItem (task);
  PPrioItem tmp_item = ready_task_by_blevel_item[task->GetID()];
  ready_task_by_blevel->RemoveItem (tmp_item);
  delete tmp_item;

     // 2.7.   Schedule the task on pid
  proc->AddTask (pid, task, ST);
     //        Update CP
//  if ((ST + task->GetBLevel()) > CP)
//    CP = ST + task->GetBLevel();

     // 2.8.   Move the tasks with t_level > PIT(pid) to global task list
  sched_time proc_idle_time = proc->GetProcReadyTime (pid);
  while (proc_ready_task[pid]->Head() != 0)
  {
    PFLB_f_Node t = (PFLB_f_Node) proc_ready_task[pid]->Head();
    if (t->GetTLevel() >= proc_idle_time)
      break;

    proc_ready_task[pid]->Dequeue();
    t->SetProcID (NONE);
    global_ready_task->Enqueue (t);
  }

     // 2.9.  Update the processor lists.
  proc_item[pid]->SetPrio (-task->GetFinishTime());
  proc_list->BalanceHeap (proc_item[pid]);

     // 2.10.   Add the new tasks (because of the current task scheduling) to
     //         the ready task lists.
  UpdateReadyTaskLists (task);

  return task;
}


void
FFF_f_Scheduling::UpdateReadyTaskLists (PFLB_f_Node task)
{
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PFLB_f_Node succ = (PFLB_f_Node) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      int sid = succ->GetID();
      succ->ComputeReadyVals();
      int p = succ->GetProcID();

      if (succ->GetTLevel() >= proc->GetProcReadyTime (p))
      {  // EP-type task
        proc_ready_task[p]->Enqueue (succ);
      }
      else
      {  // nonEP-type task
        succ->SetProcID (NONE);
        global_ready_task->Enqueue (succ);
      }
      ready_task_by_blevel_item[sid] = new PrioItem (sid, succ->GetBLevel());
      ready_task_by_blevel->Enqueue (ready_task_by_blevel_item[sid]);
    }
  }
}
