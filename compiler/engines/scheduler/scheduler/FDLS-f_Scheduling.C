/* $Id: FDLS-f_Scheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  FDLS-f  by  Andrei - Jul 2000

  Characteristics
    - equivalent to O(W(E+V)P)'s DLS by Sih and Lee
    - bounded nr. of processors
    - no duplication
    - O (V log(W) + E)
    
  Algorithm description
    prio (t,p) = BL(t) - max (EMT(t,p), PRT(p))

               = BL(t) - max (LMT(t), PRT(p)),     if p != EP(t)
                 BL(t) - max (EMT(t,p), PRT(p)),   if p = EP(t)

    The following values are used:
      LMT(t) = last message arrival time (no comm cost zeroing).
      EMT(t,p) = minimum start time, obtained by zeroing 
                 the communication costs from the P(t)
                 from which the last message arrives.
      PRT(p) = precessor ready time.
      EP(t) = t's enabeling processor (i.e., the processor t's last message 
              is sent from)

    - When the task t becomes ready, LMT(t), EMT(t,EP(t)) are computed.
    - EP tasks:
      - decreasingly sorted by EMT(t,EP(t)) - BL(t) on their enabeling 
        processors.
      - top EP tasks on each processor are decreasingly sorted by prio(t,p).
    - non-EP tasks:
      - decreasingly sorted by LMT(t).
    - processors:
      - decreasingly sorted by PRT(p).

    - The two pair candidates:
      - EP task and its enabeing processor with minimum prio(t,p).
      - non-EP task with minimum LMT(t) and processor with minimum PRT(p).
    - The pair with the highest prio(t,p) is scheduled.
*/
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "../data/DirectHeap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "FDLS-f_Node.H"
#include "Scheduling.H"
#include "FDLS-f_Scheduling.H"


   /**
      Build the task and processor priority lists.
   */
FDLS_f_Scheduling::FDLS_f_Scheduling (void)
    : Scheduling()
{
  proc_list = new DirectHeap (proc->GetNrProc());
  BL_nonEP_task_list = new Heap();
  BL_nonEP_task_item = new PPrioItem [dag->GetNrNodes()];
  prio_nonEP_task_list = new Heap();
  active_proc_list = new DirectHeap (proc->GetNrProc());
  proc_EP_task_list = new PHeap[proc->GetNrProc()];
  proc_EP_task_item = new PPrioItem [dag->GetNrNodes()];
  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    PPrioItem item = active_proc_list->GetItem (p);
    item->SetIDX (NONE);
    item->SetPrio (-MAXDOUBLE);
    proc_EP_task_list[p] = new Heap();
  }

  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PFDLS_f_Node node = (PFDLS_f_Node) dag->GetNode (nid);
    node->ComputeBLevel();
    node->InitNrUnschedPred();
    proc_EP_task_item[nid] = NULL;

    if (node->GetNrInEdges() == 0)
    {
      BL_nonEP_task_item[nid] = new PrioItem (nid, node->GetBLevel());
      BL_nonEP_task_list->Enqueue (BL_nonEP_task_item[nid]);
      prio_nonEP_task_list->Enqueue (node);
    }
  }
}

FDLS_f_Scheduling::~FDLS_f_Scheduling ()
{
  delete proc_list;
  delete BL_nonEP_task_list;
  delete prio_nonEP_task_list;
  delete active_proc_list;
  for (int p = 0; p < proc->GetNrProc(); p++)
    delete proc_EP_task_list[p];
  delete[] proc_EP_task_list;

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    delete BL_nonEP_task_item[i];
    delete proc_EP_task_item[i];
  }
  delete[] BL_nonEP_task_item;
  delete[] proc_EP_task_item;
}


void
FDLS_f_Scheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
    ScheduleTask();
}


PFDLS_f_Node
FDLS_f_Scheduling::ScheduleTask (void)
{
     // processor with minimum PRT(p) for non EP tasks.
  int non_ep_pid = ((PPrioItem) proc_list->Head())->GetIDX();
  PFDLS_f_Node non_ep_task;
  sched_time non_ep_prio;
     // non-EP task with minimum BL(t)
  int BL_non_ep_tid = ((PPrioItem) BL_nonEP_task_list->Head())->GetIDX();
  PFDLS_f_Node BL_non_ep_task = (PFDLS_f_Node) dag->GetNode (BL_non_ep_tid);
  sched_time BL_non_ep_prio = BL_non_ep_task->ComputePrio (non_ep_pid);
     // non-EP task with minimum BL(t) LMT(t) 
  PFDLS_f_Node prio_non_ep_task = (PFDLS_f_Node) prio_nonEP_task_list->Head();
  sched_time prio_non_ep_prio = prio_non_ep_task->ComputePrio (non_ep_pid);

     // EP task and its enabeing processor with minimum prio(t,p).
  PFDLS_f_Node ep_task = NULL;
  sched_time ep_prio = -MAXDOUBLE;
  int ep_pid = active_proc_list->Head()->GetIDX();
  if (ep_pid != NONE)
  {
    int nid = ((PPrioItem) proc_EP_task_list[ep_pid]->Head())->GetIDX();
    ep_task = (PFDLS_f_Node) dag->GetNode (nid);
    ep_prio = ep_task->ComputePrio (ep_pid);
  }

  if (BL_non_ep_prio > prio_non_ep_prio)
  {
    non_ep_task = BL_non_ep_task;
    non_ep_prio = BL_non_ep_prio;
  }
  else
  {
    non_ep_task = prio_non_ep_task;
    non_ep_prio = prio_non_ep_prio;
  }
  
     // select the task and processor
  PFDLS_f_Node task = NULL;
  int pid;
  if (ep_prio > non_ep_prio)
  {
    task = ep_task;
    pid = ep_pid;
  }
  else
  {
    task = non_ep_task;
    pid = non_ep_pid;
  }
  if (proc_EP_task_item[task->GetID()] != NULL)
  {
    int p = task->GetProcID();
    proc_EP_task_list[p]->RemoveItem (proc_EP_task_item[task->GetID()]);
    delete proc_EP_task_item[task->GetID()];
    proc_EP_task_item[task->GetID()] = NULL;

    PFDLS_f_Node t = (PFDLS_f_Node) proc_EP_task_list[p]->Head();
    PPrioItem item = active_proc_list->GetItem (p);
    if (t != NULL)
    {
      item->SetIDX (p);
      item->SetPrio (t->ComputePrio(p));
    }
    else
    {
      item->SetIDX (NONE);
      item->SetPrio (-MAXDOUBLE);
    }
    active_proc_list->BalanceHeap (item);
  }
  delete BL_nonEP_task_list->RemoveItem (BL_nonEP_task_item[task->GetID()]);
  BL_nonEP_task_item[task->GetID()] = NULL;
  prio_nonEP_task_list->RemoveItem (task);

     // Schedule task
  proc->AddTask (pid, task, task->ComputeStartTimeOnProc (pid));
  if (DEBUG)
  {
    for (int i = 0; i < pid; i++)
      printf ("   ");
    printf ("#%d: %d[%3.0f-%3.0f] (%3.0f)\n", pid, task->GetID(),
            task->GetStartTime(), task->GetFinishTime(),
            (ep_prio > non_ep_prio)? ep_prio: non_ep_prio);
    fflush (stdout);
  }

     // Remove the tasks with LMT > PRT(pid) from processor's lists
  sched_time prt = proc->GetProcReadyTime (pid);
  for (;;)
  {
    if (proc_EP_task_list[pid]->GetSize() == 0)
    {
      PPrioItem item = active_proc_list->GetItem (pid);
      item->SetIDX (NONE);
      item->SetPrio (-MAXDOUBLE);
      active_proc_list->BalanceHeap (item);
      break;
    }

    int nid = ((PPrioItem) proc_EP_task_list[pid]->Head())->GetIDX();
    PFDLS_f_Node t = (PFDLS_f_Node) dag->GetNode (nid);
    if (t->GetLMT() < prt)
    {
      PPrioItem item = active_proc_list->GetItem (pid);
      item->SetIDX (pid);
      item->SetPrio (t->ComputePrio(pid));
      active_proc_list->BalanceHeap (item);
      break;
    }

    delete proc_EP_task_list[pid]->Dequeue();
    proc_EP_task_item[t->GetID()] = NULL;
    t->SetProcID (NONE);
  }

     // Update the processor lists.
  PFDLS_f_Node t = (PFDLS_f_Node) proc_EP_task_list[pid]->Head();
  if (t != NULL)
  {
    PPrioItem item = active_proc_list->GetItem (pid);
    item->SetIDX (pid);
    item->SetPrio (t->ComputePrio(pid));
    active_proc_list->BalanceHeap (item);
  }

  PPrioItem item = proc_list->GetItem (pid);
  item->SetPrio (-prt);
  proc_list->BalanceHeap (item);


     // Add the new ready tasks to the task lists.
  for (unsigned int i = 0; i < task->GetNrOutEdges(); i++)
  {
    PFDLS_f_Node succ = (PFDLS_f_Node) task->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      int sid = succ->GetID();
      succ->ComputeReadyVals();

      BL_nonEP_task_item[sid] = new PrioItem (sid, succ->GetBLevel());
      BL_nonEP_task_list->Enqueue (BL_nonEP_task_item[sid]);
      prio_nonEP_task_list->Enqueue (succ);
      
      int p = succ->GetProcID();
      if (succ->GetLMT() >= proc->GetProcReadyTime (p))
      {
        proc_EP_task_item[sid] = new PrioItem (sid, succ->ComputePrio(p));
        proc_EP_task_list[p]->Enqueue (proc_EP_task_item[sid]);
        
        if (((PPrioItem) proc_EP_task_list[p]->Head())->GetIDX() == sid)
        {
          PPrioItem item = active_proc_list->GetItem (p);
          item->SetIDX (p);
          item->SetPrio (succ->ComputePrio(p));
          active_proc_list->BalanceHeap (item);
        }
      }
      else
      {
        succ->SetProcID (NONE);
      }
    }
  }

  return task;
}
