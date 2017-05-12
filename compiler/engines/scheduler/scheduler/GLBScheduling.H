/* $Id: GLBScheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __GLBSCHEDULING_H
#define __GLBSCHEDULING_H

#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "GLBClusterItem.H"


/**
  Cluster Merging to suits the number of available processors
  by  Andrei Radulescu

  Characteristics
    - O(C log(P) + E)
    
  Algorithm description
    1. Compute the arithmetic load LM[j] for each task cluster j.
    2. Sort the task clusters in the increasing order of their start time.
       Start time of a task cluster is considered the minimum start time
       of a task belonging to the task cluster.
    3. Map the task clusters according to the previous ordering.  A task
       cluster is mapped to the least loaded processor.
*/
class GLBScheduling
  : public Scheduling
{
  public:
    GLBScheduling (int task_ordering_alg);
    void Schedule (void);

  private:
    int nr_task_clusters;
    PHeap task_cluster_list;
    PGLBClusterItem *task_cluster_item;

    int nr_proc_clusters;
    PHeap proc_list;
    PPrioItem *proc_item;

    void ComputeNrTaskClusters (void);
    void MakeTaskClusterList (void);
    void MakeProcLists (void);
    void MergeTaskClusters (void);
    void PrintTaskClusters (void);
};

#endif
