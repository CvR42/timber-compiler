/* $Id: SARScheduling.H,v 1.1 2000/10/19 21:09:46 andi Exp $ */

#ifndef __SARSCHEDULING_H
#define __SARSCHEDULING_H

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "Scheduling.H"
#include "SARNode.H"


/**
  Cluster Merging to suits the number of available processors
  by  V. Sarkar \cite{Sar89}

  Characteristics
    - O (P V (V + E))
    
  Algorithm description
    1. The V tasks are sorted in a descending order of their priorities.
       In this case (ALST, as for MCP).
    2. The tasks are mapped along with their clusters to the processor
       that results in "the minimum increase of parallel time".  The
       parallel time is determined by executing the scheduled clusters
       in the physical processors and the unscheduled clusters in
       virtual processors.
*/
class SARScheduling
  : public Scheduling
{
  public:
    SARScheduling (int task_ordering_alg);
    void Schedule (void);
  
  private:
    PHeap node_list;

     /** Fields used for task ordering. */
     /// The time last task finishes execution on each processor.
  sched_time *proc_free;
     /// The time last task finishes execution on each cluster.
  sched_time *cluster_free;
     /// The priority list of ready task.
  PHeap ready_tasks;


       /** Node prio order is the length of the longest path from 
           the current node to an exit node including communication. */
    void ComputeNodeOrderPrios (void);

     /** Node prio order is the length of the longest path from 
           the current node to an exit node including communication.
         Build a priority list of tasks using the above prio. */
    void MakeNodeList (void);

     /** The clusters are mapped when mapping the first task in the
         cluster.  The processor is chosen such that the minimum
         increase in parallel time is achieved. */
    void MapClusters (void);

     /** Map all the tasks in the cluster of 'node' to processor 'pid'. */
    void MapCluster (PSARNode node, int pid);

    void InitForTaskOrdering (void);
    sched_time OrderTasks (int cluster, int pid);
    void PrintClusters (void);
};

#endif
