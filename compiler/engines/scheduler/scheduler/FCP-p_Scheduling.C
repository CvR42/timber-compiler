/* $Id: FCP-p_Scheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  FCP-p (Fast Critical Path)  by  Andrei - Nov 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(P) + E)
    
  Algorithm description
    1.   Compute bottom levels (b_level) for all tasks.
    2.   Repeat until all tasks are scheduled.
    2.1.   Find the processor to which the next node will be mapped,
             as the processor becoming idle the earliest.
    2.2.   Find the task with the minimum b_level.
             The first X readys are sorted by their b_level,
             the other ready tasks are kept in a FIFO list
    2.3.   Schedule the chosen task to the processor.
    2.4.   Add the new tasks (because of the current task scheduling) to
             the ready task lists.
*/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/PseudoHeap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "FCP-p_Node.H"
#include "Scheduling.H"
#include "FCP-p_Scheduling.H"


   /**
      Build the task and processor priority lists.
   */
FCP_p_Scheduling::FCP_p_Scheduling (double pseudo_heap_size)
    : Scheduling()
{
  ready_task = new PseudoHeap (pseudo_heap_size * proc->GetNrProc());

  mean_flops = 0;
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PFCP_p_Node node = (PFCP_p_Node) dag->GetNode (nid);
    node->ComputeBLevel();
    node->InitNrUnschedPred();

    if (node->GetNrInEdges() == 0)
      ready_task->Enqueue (node);

    mean_flops += node->GetFlops();
  }
  mean_flops /= dag->GetNrNodes();

  proc_list = new Heap();
  proc_item = new PPrioItem [proc->GetNrProc()];
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    double prio = - mean_flops / proc->GetProcSpeed (pid);
    proc_item[pid] = new PrioItem (pid, prio, proc->GetProcSpeed(pid));
//    proc_item[pid] = new PrioItem (pid, 0);
    proc_list->Enqueue (proc_item[pid]);
  }
}

FCP_p_Scheduling::~FCP_p_Scheduling ()
{
  delete ready_task;

  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    delete proc_item[pid];
  }
  delete proc_item;
  delete proc_list;
}


   /**
      While there are unscheduled tasks, pick the first processor
      that become idle and map a task to it.
   */
void
FCP_p_Scheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
  {
    PFCP_p_Node node = ScheduleTask();
    UpdateReadyTaskList (node);
  }
}


   /**
      2.1. Select the task with the minimum b_level.
             The first X readys are sorted by their b_level,
             the other ready tasks are kept in a FIFO list
      2.2. Select the processor to which the next node will be mapped.
           The candidates are:
             - the processor becoming idle the earliest.
             - the processor to which is mapped the parent
               with the most costly link.
           The processor is the one on which the task starts the earliest.    
      2.3. Schedule the chosen task to the processor.
      2.4. Add the new ready tasks (because of the current task scheduling) to
             the ready task lists.
   */
PFCP_p_Node
FCP_p_Scheduling::ScheduleTask (void)
{
     // 2.1. Select the task with the minimum b_level.
  PFCP_p_Node node = (PFCP_p_Node) ready_task->Dequeue();

     // 2.1. Select the processor to which the next node will be mapped.
     // First candidate processor is the one becoming idle the earliest.
  int pid = ((PPrioItem) proc_list->Head())->GetIDX();
  sched_time ST = node->ComputeStartTimeOnProc (pid);
  sched_time FT = ST + node->GetExecTime (pid);

     // Second candidate processor is the one where is mapped the parent
     // with the highest costly link.
  int pid2 = NONE;
  sched_time msg_arrival_time = 0;
  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) node->GetInEdge (i);
    PFCP_p_Node pred_node = (PFCP_p_Node) in_edge->GetFromNode();
    sched_time aux_msg_arrival_time = pred_node->GetFinishTime() +
                                      in_edge->GetCommTime();

    if (aux_msg_arrival_time > msg_arrival_time)
    {
      msg_arrival_time = aux_msg_arrival_time;
      pid2 = pred_node->GetProcID();
    }
  }

     // Select the processor on which the task starts the earliest.
  if (pid2 != NONE)
  {
    sched_time ST2 = node->ComputeStartTimeOnProc (pid2);
    sched_time FT2 = ST2 + node->GetExecTime (pid2);
    if (FT2 < FT)
    {
      ST = ST2;
      pid = pid2;
    }
  }
  
     // 2.3. Schedule the chosen task to the processor.
  proc->AddTask (pid, node, ST);
  if (DEBUG)
  {
    printf ("Add %d to %d at %7.1f\n", node->GetID(), pid, ST);
    fflush (stdout);
  }

     // Update the processor heap
  PPrioItem item = proc_item[node->GetProcID()];
//  item->SetPrio (-proc->GetProcReadyTime (node->GetProcID()));
  item->SetPrio (-proc->GetProcReadyTime (node->GetProcID()) - 
                 mean_flops / proc->GetProcSpeed (pid));
  proc_list->BalanceHeap (item);

  return node;
}


   /**
      2.4. Add the new tasks (because of the current task scheduling) to
           the proper task lists.
   */
void
FCP_p_Scheduling::UpdateReadyTaskList (PFCP_p_Node node)
{
  for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
  {
    PFCP_p_Node succ = (PFCP_p_Node) node->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
      ready_task->Enqueue (succ);
  }
}
