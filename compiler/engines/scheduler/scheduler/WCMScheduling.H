/* $Id: WCMScheduling.H,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#ifndef __WCMSCHEDULING_H
#define __WCMSCHEDULING_H

#include "../data/Heap.H"
#include "Scheduling.H"


/**
  Cluster Merging to suits the number of available processors
  by  Tao Yang - \cite{Yan93}.

  Characteristics
    - O(V log(V))
    
  Algorithm description
    1. Compute the arithmetic load LM[j] for each cluster j.  Determine
       the average load A = Sum (LM[j]) / P.
    2. Sort the clusters in the increasing order of their loads.
    3. Assign virtual processors to those clusters with LM[j] >= A.
    4. Use wrap or reflection mapping for the remaining clusters.
         Wrap maping:  Virtual processor of cluster j is (j-1) mod q,
                       where q is the number of remaining processors
                       after Step 3.
*/
class WCMScheduling
  : public Scheduling
{
  public:
    WCMScheduling (int task_ordering_alg);

     /** Schedule tasks in the DAG according to the current algorithm. */
    void Schedule (void);


  private:
    PHeap cluster_list;
    double *load;


     /** 1. Compute the arithmetic load LM[j] for each cluster j.  Determine
            the average load A = Sum (LM[j]) / P.
         2. Sort the clusters in the increasing order of their loads.
         3. Assign virtual processors to those clusters with LM[j] >= A.
         4. Use wrap or reflection mapping for the remaining clusters.
            Wrap maping:  Virtual processor of cluster j is (j-1) mod q,
                          where q is the number of remaining processors
                          after Step 3. */
    void MapClusters (void);

     /** Add all the nodes to the heap reprezenting the priority list.
          - ordering criteria: high load => high priority.
         Return value:
          - the priority list. */
    void MakeClusterList (void);

    void ComputeLoads (void);
    double ComputeAvrgLoad (void);
};

#endif
