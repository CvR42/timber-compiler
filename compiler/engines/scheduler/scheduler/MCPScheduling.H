/* $Id: MCPScheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __MCPSCHEDULING_H
#define __MCPSCHEDULING_H

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "MCPNode.H"
#include "Scheduling.H"


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
class MCPScheduling :
  public Scheduling
{
  public:
     /// Schedule all the tasks in the DAG according to the current algorithm.
    void Schedule (void);

  private:
     /// Compute ALST (Absolute Latest Start Time) for all nodes.
    void ComputeALSTs (void);
     /** Calculate levels (the greatest number of nodes in a path from
         an entry node) for all nodes. */
    void ComputeLevels (void);

     /** Add all the nodes to the heap reprezenting the priority list.
          - ordering criteria: low ALST => high priority.
         Return value:
          - the priority list. */
    PHeap MakePrioList (void);

     /** Find the processor where this node can start the earliest.
         Schedule the task to the found processor. */
    void ScheduleTask (PMCPNode node);

     /** Compute the start time of this node on a processor
         The start time is determined by 2 factors:
           - task dependencies (a task cannot start its execution until
             it has received all the data it needs)
           - processor utilization (a task cannot start its execution
             if the processor is busy)
         Return value:
           - the start time of this node on 'pid' */
  sched_time GetStartTimeOnProc (PMCPNode node, int pid);
};

#endif
