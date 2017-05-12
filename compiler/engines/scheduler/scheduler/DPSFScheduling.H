/* $Id: DPSFScheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __DPSFSCHEDULING_H
#define __DPSFSCHEDULING_H

#include "../data/Sched.H"
#include "../data/List.H"
#include "DPSNode.H"
#include "Scheduling.H"


/**
  DPS_F (Decisive Path Scheduling)  by  Park, Shirazi, Marquis and Choo - 1997

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
class DPSFScheduling :
  public Scheduling
{
  public:
     /// Schedule all the tasks in the DAG according to the current algorithm.
    void Schedule (void);

  private:
    PList task_list;

     /** Compute top levels for all nodes. */
    void ComputeTopLevels (void);

     /** Build the task list. */
    void BuildTaskList (void);
    void AddTask (PDPSNode node);

     /** Find the processor where this node can start the earliest.
         Schedule the task to the found processor. */
    void ScheduleTask (PDPSNode node);

     /** Compute the start time of this node on a processor
         The start time is determined by 2 factors:
           - task dependencies (a task cannot start its execution until
             it has received all the data it needs)
           - processor utilization (a task cannot start its execution
             if the processor is busy)
         Return value:
           - the start time of this node on 'pid' */
  sched_time GetStartTimeOnProc (PDPSNode node, int pid);
};

#endif
