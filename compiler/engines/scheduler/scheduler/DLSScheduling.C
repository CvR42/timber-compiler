/* $Id: DLSScheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  DLS (Dynamic Level Scheduling)  by  Sih, Lee - feb 93

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V^2 P)

  Algorithm description
    - DL(t,p) = BL(t) - max (EMT(t,p), PRT(p))
    - the task t with the maximum DL(t,p) is scheduled on p.

  Implementation:
    - The set of ready tasks is a usual list.  It is not necessarily
      to sort the ready tasks.
*/

#include <stdio.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/List.H"
#include "../dag/DAG.H"
#include "../dag/DAGEdge.H"
#include "Scheduling.H"
#include "DSA_Node.H"
#include "DLSScheduling.H"


   /**
      Build the task and processor priority lists.
   */
DLSScheduling::DLSScheduling (void)
    : Scheduling()
{
  ready_tasks_list = new List();

  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    DSA_Node *node = (DSA_Node*) dag->GetNode (nid);
    node->ComputeBLevel();
    node->InitNrUnschedPred();

    if (node->GetNrInEdges() == 0)
      ready_tasks_list->AddToBack (node);
  }
}

DLSScheduling::~DLSScheduling ()
{
  delete ready_tasks_list;
}


   /**
      While there are unscheduled tasks, pick the first processor
      that become idle and map a task to it.
   */
void
DLSScheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
  {
    ScheduleTask();
  }
}


   /**
      The algorithm computes the priority of every task
      that belongs to the ready task list on every idle procesor.
      The tasks and processor with the highest priority 
      are selected for scheduling.
   */
void
DLSScheduling::ScheduleTask (void)
{
  DSA_Node *node = NULL;
  sched_time prio = NONE;
  int pid = NONE;

  DSA_Node *n = (DSA_Node*) ready_tasks_list->GetFirst();
  for (; n != NULL; n = (DSA_Node*) n->GetNextItem())
  {
    for (int p = 0; p < proc->GetNrProc(); p++)
    {
      sched_time crt_prio = n->GetBLevel() - n->ComputeStartTimeOnProc(p);

      if ((node == NULL) || (crt_prio > prio))
      {
        node = n;
        prio = crt_prio;
        pid = p;
      }
    }
  }
  ready_tasks_list->RemoveItem (node);
  
     // Schedule the chosen task to the processor.
  proc->AddTask (pid, node, node->ComputeStartTimeOnProc(pid));
  if (DEBUG)
  {
    for (int i = 0; i < pid; i++)
      printf ("   ");
    printf ("#%d: %d[%3.0f-%3.0f] (%3.0f)\n", pid, node->GetID(),
            node->GetStartTime(), node->GetFinishTime(), prio);
    fflush (stdout);
  }

     // Add the new ready tasks to the ready task lists.
  UpdateReadyTaskList (node);
}


   /**
      Add the new ready tasks (because of the current task scheduling).
   */
void
DLSScheduling::UpdateReadyTaskList (DSA_Node *node)
{
  for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
  {
    DSA_Node *succ = (DSA_Node*) node->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
    {
      succ->ComputeReadyVals();
      ready_tasks_list->AddToBack (succ);
    }
  }
}
