/* $Id: LLBScheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

/**
  Cluster Merging to suits the number of available processors
  by  Radulescu, van Gemund, Lin - 98

  Characteristics
    - O ()
    
  Algorithm description
    1.   Compute b_level as the priority for each task.
    2.   Repeat until all tasks are scheduled.
    2.1.   Find the processor to which the next node will be mapped,
             as the processor becoming idle the earliest.
    2.2.   Find the mapped task with the highest priority.
    2.3.   Find the unmapped task with the highest priority.
    2.4.   Between the above two tasks, choose the one that start the earliest.
 ?  2.5.   If no such task exists, choose the most prioritar task mapped on
             one of the other processors.
    2.6.   Schedule the chosen task to the processor.
    2.7.   If the task was not already mapped, map also its own cluster.
    2.8.   Add the new tasks (because of the current task scheduling) to
             the proper task lists.
*/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "LLBNode.H"
#include "Scheduling.H"
#include "LLBScheduling.H"


   /**
      Build the task and processor priority lists.
   */
LLBScheduling::LLBScheduling (void)
    : Scheduling()
{
  all_ready_unmapped_tasks = new Heap();
  all_ready_unmapped_task_item = new PPrioItem [dag->GetNrNodes()];
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PLLBNode node = (PLLBNode) dag->GetNode (i);
    node->InitNrUnschedPred();
    node->ComputeBLevel();

    if (node->GetNrInEdges() == 0)
    {
      all_ready_unmapped_task_item[i] = new PrioItem (node->GetID(),
                                                      node->GetPrio());
      all_ready_unmapped_tasks->Enqueue (all_ready_unmapped_task_item[i]);
    }
    else
    {
      all_ready_unmapped_task_item[i] = NULL;
    }
  }

  ready_mapped_tasks = new PHeap [proc->GetNrProc()];
  ready_mapped_task_item = new PPrioItem [dag->GetNrNodes()];
  proc_list = new Heap();
  proc_item = new PPrioItem [proc->GetNrProc()];
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    ready_mapped_tasks[pid] = new Heap();

    proc_item[pid] = new PrioItem (pid, 0);
    proc_list->Enqueue (proc_item[pid]);
  }

  all_ready_mapped_tasks = new Heap();
  all_ready_mapped_task_item = new PPrioItem [dag->GetNrNodes()];
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    all_ready_mapped_task_item[i] = NULL;
    ready_mapped_task_item[i] = NULL;
  }
}


   /**
      While there are unscheduled tasks, pick the first processor
      that become idle and map a task to it.
   */
void
LLBScheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  while (unscheduled_tasks > 0)
  {
    int pid = ((PPrioItem) proc_list->Head())->GetIDX();
    PLLBNode node = ScheduleTaskOnProc (pid);

    PPrioItem item = proc_item[node->GetProcID()];
    item->SetPrio (-proc->GetProcReadyTime (node->GetProcID()));
    proc_list->BalanceHeap (item);

    UpdateTaskLists (node);

    unscheduled_tasks--;
    if (DEBUG)
      printf ("  %d left\n", unscheduled_tasks);
  }
}


   /**
      2.2. Find the most prioritar mapped task.
      2.3. Find the most prioritar unmapped task.
      2.4. Between the above 2 tasks, choose the one that start the earliest.
   ?  2.5. If no such task exists, choose the most prioritar task mapped on
            one of the other processors.
      2.6. Schedule the chosen task to the processor.
      2.7. If the task was not already mapped, map also its own cluster.
   */
PLLBNode
LLBScheduling::ScheduleTaskOnProc (int pid)
{
  PLLBNode node = NULL;
  sched_time ST;
  PPrioItem task_item;

     // 2.2. Find the mapped task with the highest priority.
  PLLBNode mapped_node = NULL;
  sched_time mapped_ST = MAX_TIME;
  if ((task_item = (PPrioItem) ready_mapped_tasks[pid]->Head()) != NULL)
  {
    mapped_node = (PLLBNode) dag->GetNode (task_item->GetIDX());
    mapped_ST = mapped_node->ComputeStartTimeOnProc (pid); //GetTLevel();
  }

     // 2.3. Find the unmapped task with the highest priority.
  PLLBNode unmapped_node = NULL;
  sched_time unmapped_ST = MAX_TIME;
  if ((task_item = (PPrioItem) all_ready_unmapped_tasks->Head()) != NULL)
  {
    unmapped_node = (PLLBNode) dag->GetNode (task_item->GetIDX());
    unmapped_ST = unmapped_node->ComputeStartTimeOnProc (pid);
  }

     // 2.4. Between the above 2 tasks, choose the one that start the earliest.
  if ((mapped_node != NULL) || (unmapped_node != NULL))
  {
    if (mapped_ST <= unmapped_ST)
    {
      if (DEBUG)
        printf ("M ");
      node = mapped_node;
      ST = mapped_ST;
      int nid = node->GetID();

      ready_mapped_tasks[pid]->Dequeue();
      delete ready_mapped_task_item[nid];
      ready_mapped_task_item[nid] = NULL;

      all_ready_mapped_tasks->RemoveItem (all_ready_mapped_task_item[nid]);
      delete all_ready_mapped_task_item[nid];
      all_ready_mapped_task_item[nid] = NULL;
    }
    else
    {
      if (DEBUG)
        printf ("U ");
      node = unmapped_node;
      ST = unmapped_ST;
      int nid = node->GetID();

      all_ready_unmapped_tasks->Dequeue();
      delete all_ready_unmapped_task_item[nid];
      all_ready_unmapped_task_item[nid] = NULL;

         // 2.7. The task was not mapped, so map also its own cluster.
      MapTaskCluster (node->GetVirtualProcessor(), pid);
    }
  }
     // 2.5. If no such task exists, choose the most prioritar task mapped on
     //       one of the other processors.
  else
  {
    if (DEBUG)
      printf ("3 ");
    task_item = (PPrioItem) all_ready_mapped_tasks->Dequeue();
    node = (PLLBNode) dag->GetNode (task_item->GetIDX());
       // v.1 modify pid;  v.2 use the same pid
    pid = node->GetProcID();
    ST = node->ComputeStartTimeOnProc (pid);
    int nid = node->GetID();

    delete all_ready_mapped_task_item[nid];
    all_ready_mapped_task_item[nid] = NULL;

    ready_mapped_tasks[pid]->RemoveItem (ready_mapped_task_item[nid]);
    delete ready_mapped_task_item[nid];
    ready_mapped_task_item[nid] = NULL;
  }

     // 2.6. Schedule the chosen task to the processor.
  proc->AddTask (pid, node, ST);
  if (DEBUG)
  {
    printf ("Add %d to %d at %7.1f...", node->GetID(), pid, ST);
    fflush (stdout);
  }

  return node;
}


   /**
      2.8. Add the new tasks (because of the current task scheduling) to
           the proper task lists.
   */
void
LLBScheduling::UpdateTaskLists (PLLBNode node)
{
  for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
  {
    PLLBNode succ = (PLLBNode) node->GetSuccNode (i);
    int succ_id = succ->GetID();
    int succ_pid = succ->GetProcID();
    if (succ->DecNrUnschedPred() == 0)
    {
//        succ->SetTLevel (ComputeStartTimeOnProc (succ_pid));
      sched_time prio = succ->GetPrio();

      if (succ_pid == NONE)
      {
        all_ready_unmapped_task_item[succ_id] = new PrioItem (succ_id, prio);
        all_ready_unmapped_tasks->Enqueue (all_ready_unmapped_task_item[succ_id]);
      }
      else
      {
        ready_mapped_task_item[succ_id] = new PrioItem (succ_id, prio);
        ready_mapped_tasks[succ_pid]->Enqueue(ready_mapped_task_item[succ_id]);
        all_ready_mapped_task_item[succ_id] = new PrioItem(succ_id, prio);
        all_ready_mapped_tasks->Enqueue(all_ready_mapped_task_item[succ_id]);
      }
    }
  }
}


   /**
      Map all the tasks belonging to a cluster (cluster) to a processor (pid).
   */
void
LLBScheduling::MapTaskCluster (int cluster, int pid)
{
  if (cluster == NONE)
    return;

  for (PProcListItem nl = (PProcListItem) old_proc->GetFirstTask(cluster);
       nl != NULL; nl = (PProcListItem) nl->GetNextItem())
  {
    PLLBNode node = (PLLBNode) nl->GetCrtNode();
    int nid = node->GetID();

    node->SetProcID (pid);

    if (all_ready_unmapped_task_item[nid] != NULL)
    {
      all_ready_unmapped_tasks->RemoveItem (all_ready_unmapped_task_item[nid]);
      delete all_ready_unmapped_task_item[nid];
      all_ready_unmapped_task_item[nid] = NULL;

      sched_time prio = node->GetPrio();

      ready_mapped_task_item[nid] = new PrioItem (nid, prio);
      ready_mapped_tasks[pid]->Enqueue (ready_mapped_task_item[nid]);
      all_ready_mapped_task_item[nid] = new PrioItem (nid, prio);
      all_ready_mapped_tasks->Enqueue (all_ready_mapped_task_item[nid]);
    }
  }
}
