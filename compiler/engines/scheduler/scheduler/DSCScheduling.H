/* $Id: DSCScheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef __DSCSCHEDULING_H
#define __DSCSCHEDULING_H

#include "../data/Heap.H"
#include "Scheduling.H"
#include "DSCNode.H"


/**
  DSC (Dominant Sequence Clustering Algorithm)  by  Yang, Gerasoulis - 1994.

  Characteristics \\
    - unbounded number of processors \\
    - no duplication \\
    - O((V+E) log(V)) \\
    
  Algorithm description \\
    - The priorities of a task are an approximation of the length of
      the longest path the task belong to.  It is guaranteed that the
      tasks belonging to the critical path are always prefered.  The
      priorities are calculated at each step, but only for ready tasks
      that can have their priority changed. \\
    - The cluster chosen is that which determines the minimum start time. \\
    - If the task to be scheduled does not belong to a critical path
      the cluster is selected such that there will be no delay to the
      start time of a task belonging to a critical path.  This is done
      by tentatively choosing a cluster for a partial ready task
      belonging to a critical path and blocking that cluster as long
      as the critical task is not scheduled.
*/
class DSCScheduling
  : public Scheduling
{
  public:
    DSCScheduling (void);
    ~DSCScheduling ();

    void Schedule (void);


  private:
    PHeap free_tasks;
    PHeap partial_free_tasks;
    sched_time start_time, comm_start_time;

    int FindCluster (PDSCNode node, int DSRW_cluster, bool want_mapping);
    int GetDestCluster (PDSCNode node, int DSRW_cluster);
    sched_time TryMappingPred (PDSCNode node, int proc_id);
    sched_time GetStartTime (PDSCNode node, int proc_id, sched_time level);
    sched_time MakeCandidateTasks (PDSCNode node, int proc_id,
                                   PProcList candidate_tasks);
    PHeap MakePredList (PDSCNode node);
};

#endif
