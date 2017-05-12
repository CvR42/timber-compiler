/* $Id: ERTScheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  ERT (Earliest Ready Task)  by Lee, Hwang, Chow, Anger - Jun 88

  Characteristics
    - bounded nr. of processors
    - no duplication
    - O(V^2 P)

  Algorithm description
    - The ERT algorithm uses a set called _ready task set_. A task is said 
      to be _ready_ when all its predecessors are scheduled.
    - The algorithm calculates the earliest finish time of every task that
      belongs to the ready task list on every idle procesor. The tasks are
      assigned in the ascending order of their earliest finish times.
    - The set of ready tasks and as each task finishes its
      execution.

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
#include "ERTNode.H"
#include "ERTScheduling.H"


   /**
      Build the task and processor priority lists.
   */
ERTScheduling::ERTScheduling (void)
    : Scheduling()
{
  ready_tasks_list = new List();

  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    PERTNode node = (PERTNode) dag->GetNode (nid);
    node->InitNrUnschedPred();

    if (node->GetNrInEdges() == 0)
      AddReadyTask (node);
  }
}

ERTScheduling::~ERTScheduling ()
{
  delete ready_tasks_list;
}


   /**
      While there are unscheduled tasks, pick the first processor
      that become idle and map a task to it.
   */
void
ERTScheduling::Schedule (void)
{
  int unscheduled_tasks = dag->GetNrNodes();
  for (; unscheduled_tasks > 0; unscheduled_tasks--)
  {
    ScheduleTask();
  }
}


   /**
      The algorithm calculates the earliest start time of every task
      that belongs to the ready task list on every idle procesor.
      The tasks are assigned in the ascending order of their
      earliest start times.
   */
void
ERTScheduling::ScheduleTask (void)
{
  PERTNode node = NULL;
  sched_time earliest_start_time = NONE;
  sched_time earliest_finish_time = NONE;
  int pid = NONE;

  PERTNode n = (PERTNode) ready_tasks_list->GetFirst();
  for (; n != NULL; n = (PERTNode) n->GetNextItem())
  {
    for (int p = 0; p < proc->GetNrProc(); p++)
    {
      sched_time ST = n->ComputeStartTimeOnProc (p);
      sched_time FT = ST + n->GetExecTime (p);

      if ((earliest_finish_time == NONE) || (FT < earliest_finish_time))
      {
        node = n;
        earliest_start_time = ST;
        earliest_finish_time = FT;
        pid = p;
      }
    }
  }
  ready_tasks_list->RemoveItem (node);
  
     // Schedule the chosen task to the processor.
  proc->AddTask (pid, node, earliest_start_time);
  if (DEBUG)
  {
    for (int i = 0; i < pid; i++)
      printf ("              ");
    printf ("%d[%3.0f-%3.0f]\n", node->GetID(),
            earliest_start_time,
            earliest_start_time + node->GetExecTime(pid));
    fflush (stdout);
  }

     // Add the new ready tasks to the ready task lists.
  UpdateReadyTaskList (node);
}


   /**
      Add the new tasks (because of the current task scheduling) to
      the proper task lists.
   */
void
ERTScheduling::UpdateReadyTaskList (PERTNode node)
{
  for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
  {
    PERTNode succ = (PERTNode) node->GetSuccNode (i);

    if (succ->DecNrUnschedPred() == 0)
      AddReadyTask (succ);
  }
}


void
ERTScheduling::AddReadyTask (PERTNode node)
{
  ready_tasks_list->AddToBack (node);
}
