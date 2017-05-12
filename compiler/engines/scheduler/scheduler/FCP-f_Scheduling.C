/* $Id: FCP-f_Scheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  FCP-f (Fast Critical Path)  by  Andrei - Nov 98

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V log(W) + E)
    
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
#include "../al/mapint.h"

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "../processors/Processors.H"
#include "FCP-f_Node.H"
#include "Scheduling.H"
#include "FCP-f_Scheduling.H"


   /**
      Build the task and processor priority lists.
   */
FCP_f_Scheduling::FCP_f_Scheduling (void)
    : Scheduling()
{
  mean_flops = 0;
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PFCP_f_Node node = (PFCP_f_Node) dag->GetNode (nid);
    node->ComputeBLevel();
    node->InitNrUnschedPred();

    if (node->GetNrInEdges() == 0)
      ready_task.push (node);

    mean_flops += node->GetFlops();
  }
  mean_flops /= dag->GetNrNodes();

  proc_list = new mapint<sched_time> (proc->GetNrProc());
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    (*proc_list)[pid] = mean_flops / proc->GetProcSpeed (pid);
  }
  proc_list->make_heap();
}

FCP_f_Scheduling::~FCP_f_Scheduling ()
{}


   /**
      While there are unscheduled tasks, pick the first processor
      that become idle and map a task to it.
   */
void
FCP_f_Scheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
  {
    PFCP_f_Node node = ScheduleTask();
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
PFCP_f_Node
FCP_f_Scheduling::ScheduleTask (void)
{
     // 2.1. Select the task with the minimum b_level.
  PFCP_f_Node node = ready_task.top();
  ready_task.pop();

     // 2.1. Select the processor to which the next node will be mapped.
     // First candidate processor is the one becoming idle the earliest.
  int pid = proc_list->top_ref();
  sched_time ST = node->ComputeStartTimeOnProc (pid);
  sched_time FT = ST + node->GetExecTime (pid);

     // Second candidate processor is the one where is mapped the parent
     // with the highest costly link.
  int pid2 = NONE;
  sched_time msg_arrival_time = 0;
  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) node->GetInEdge (i);
    PFCP_f_Node pred_node = (PFCP_f_Node) in_edge->GetFromNode();
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
  proc->AddTask (pid, node, node->ComputeStartTimeOnProc(pid));
//AAA
  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) node->GetInEdge (i);
    PFCP_f_Node pred_node = (PFCP_f_Node) in_edge->GetFromNode();
    pred_node->SetFT (pred_node->GetFT() + in_edge->GetCommTime());
  }
//AAA
  if (DEBUG)
  {
    printf ("Add %d(%f) to %d at %7.1f\n", node->GetID(), node->GetPrio(), 
            pid, ST);
    fflush (stdout);
  }

     // Update the processor heap
  (*proc_list)[pid] = proc->GetProcReadyTime (node->GetProcID()) +
                      mean_flops / proc->GetProcSpeed (pid);
  proc_list->make_heap();

  return node;
}


   /**
      2.4. Add the new tasks (because of the current task scheduling) to
           the proper task lists.
   */
void
FCP_f_Scheduling::UpdateReadyTaskList (PFCP_f_Node node)
{
  for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
  {
    PFCP_f_Node succ = (PFCP_f_Node) node->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
      ready_task.push (succ);
  }
}
