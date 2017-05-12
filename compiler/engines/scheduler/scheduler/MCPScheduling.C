/* $Id: MCPScheduling.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

/**
  MCP (Most Critical Path)  by  Wu, Gajski - Nov 88

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V^2 log(V))
    
  Algorithm description
    - MCP schedules tasks in the order of their ALST (Absolute Lastest
      Start Time).  ALST is called by Wu and Gajski ALAP (As Late As
      Possible) time.
    - The chosen task is scheduled to the processor that allows its
      earliest starting time.

  Implementation details
    - The priority list is implemented as a heap.
*/

#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "MCPNode.H"
#include "Scheduling.H"
#include "MCPScheduling.H"



/* Schedule all the tasks in the DAG according to the current algorithm. */
void
MCPScheduling::Schedule (void)
{
  PHeap prio_list;

  ComputeALSTs();
  ComputeLevels();

  prio_list = MakePrioList();                 // based on ALSTs


  for (PMCPNode node = (PMCPNode) prio_list->Dequeue();
       node != NULL;  node = (PMCPNode) prio_list->Dequeue())
  {
    ScheduleTask (node);
  }

  delete prio_list;
}



   /** Compute ALST (Absolute Latest Start Time) for all nodes. */
void
MCPScheduling::ComputeALSTs (void)
{
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PMCPNode node = (PMCPNode) dag->GetNode(i);
    node->ComputeALST();
  }
}

   /** Calculate levels (the greatest number of nodes in a path from
       an entry node) for all nodes. */
void
MCPScheduling::ComputeLevels (void)
{
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PMCPNode node = (PMCPNode) dag->GetNode(i);
    node->ComputeLevel();
  }
}


   /** Add all the nodes to the heap reprezenting the priority list.
         - ordering criteria: low ALST => high priority.
       Return value:
         - the priority list. */
PHeap
MCPScheduling::MakePrioList (void)
{
  PHeap prio_list = new Heap();

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    prio_list->Enqueue ((PMCPNode) dag->GetNode(i));
  }

  return prio_list;
}


   /** Find the processor where this node can start the earliest.
       Schedule the task to the found processor. */
void
MCPScheduling::ScheduleTask (PMCPNode node)
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
    printf ("Add %d(%.1f) to %d at %7.1f\n",
            node->GetID(), node->ComputeALST(), assigned_pid, min_start_time);
}
