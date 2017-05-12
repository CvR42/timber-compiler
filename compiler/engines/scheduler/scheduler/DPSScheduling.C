/* $Id: DPSScheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  DPS (Decisive Path Scheduling)  by  Park, Shirazi, Marquis and Choo - 1997

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V*log(V) + E)
    
  Algorithm description
    - The DP (Decisive Path) of a node is defined as the path which
      determines the node's top level.
    - DPS schedules first the CPN (Critical Path Nodes), then
      IBN (Input Branche Nodes) and OBN (Out Branch Nodes).
    - IBN belonging to the DP are preferred.  That is, the nodes with
      the highest (top level + computation cost) are preferred.
    - The tasks are recursively scheduled starting from the CPN.
    - The chosen task is scheduled to the processor that allows its
      earliest starting time.

  Implementation details
*/

#include <stdio.h>
#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/List.H"
#include "../data/Heap.H"
#include "DPSNode.H"
#include "Scheduling.H"
#include "DPSScheduling.H"



/* Schedule all the tasks in the DAG according to the current algorithm. */
void
DPSScheduling::Schedule (void)
{
  ComputeTopLevels();
  BuildTaskList();

  for (PDPSNode node = (PDPSNode) task_list->DequeueFirst();
       node != NULL;  node = (PDPSNode) task_list->DequeueFirst())
  {
    ScheduleTask (node);
  }

  delete task_list;
}



   /** Compute Top Levels for all nodes. */
void
DPSScheduling::ComputeTopLevels (void)
{
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PDPSNode node = (PDPSNode) dag->GetNode(i);
    node->ComputeTopLevel();
  }
}


   /** Build the task list. */
void
DPSScheduling::BuildTaskList (void)
{
  task_list = new List();
  int nid = dag->GetNrNodes() - 1;

  AddTask ((PDPSNode) dag->GetNode(nid));
}

void
DPSScheduling::AddTask (PDPSNode node)
{
  PHeap pred_list = new Heap (node->GetNrInEdges());

  if (DEBUG)
    printf ("Adding task %d\n", node->GetID());

  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDPSNode pred_node = (PDPSNode) node->GetPredNode (i);
    pred_list->Enqueue (pred_node);
  }

  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDPSNode pred_node = (PDPSNode) pred_list->Dequeue();
    if (pred_node->InTaskList() == false)
      AddTask (pred_node);
  }

  task_list->AddToBack (node);
  node->SetInTaskList();
  delete pred_list;
}

  
   /** Find the processor where this node can start the earliest.
       Schedule the task to the found processor. */
void
DPSScheduling::ScheduleTask (PDPSNode node)
{
  sched_time min_start_time = NONE, t;
  int assigned_pid = 0;

     // Find the processor where this node can start the earliest.
  for (int pid = 0; pid < proc->GetNrProc(); pid++)
  {
    t = node->ComputeStartTimeOnProc (pid);

    if ((min_start_time == NONE) || (t < min_start_time))
    {
      min_start_time = t;
      assigned_pid = pid;
    }
  }

     // Add this node after the previous mapped nodes on 'pid'.
     // Set the theoretical start time of this node on 'pid'.
  proc->AddTask (assigned_pid, node, min_start_time);
  if (DEBUG)
  {
    printf ("Add %d to %d at %7.1f\n", node->GetID(),
                                       assigned_pid, min_start_time);
    fflush (stdout);
  }
}
