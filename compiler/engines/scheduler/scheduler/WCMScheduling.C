/* $Id: WCMScheduling.C,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "../dag/CMrgDAGNode.H"
#include "WCMNode.H"
#include "Scheduling.H"
#include "WCMScheduling.H"

/*
  Cluster Merging to suits the number of available processors
  by  Tao Yang - \cite{Yan93}

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

WCMScheduling::WCMScheduling (int task_ordering_alg)
    : Scheduling (task_ordering_alg)
{
}
  

   /** Schedule tasks in the DAG according to the current algorithm. */
void
WCMScheduling::Schedule (void)
{
  MapClusters();
  OrderTasks();
}


   /** 1. Compute the arithmetic load LM[j] for each cluster j.  Determine
          the average load A = Sum (LM[j]) / P.
       2. Sort the clusters in the increasing order of their loads.
       3. Assign virtual processors to those clusters with LM[j] >= A.
       4. Use wrap or reflection mapping for the remaining clusters.
          Wrap maping:  Virtual processor of cluster j is (j-1) mod q,
                        where q is the number of remaining processors
                        after Step 3. */
void
WCMScheduling::MapClusters (void)
{
  int limit;
  int *mapping = new int [old_proc->GetNrProc()];

     // 1. Compute the arithmetic load LM[j] for each cluster j.  Determine
     //    the average load A = Sum (LM[j]) / P.
  ComputeLoads();
  double avrg_load = ComputeAvrgLoad();

     // 2. Sort the clusters in the increasing order of their loads.
  MakeClusterList();

     // 3. Assign virtual processors to those clusters with LM[j] >= A.
  PPrioItem item = NULL;
  for (limit = proc->GetNrProc() - 1; limit >= 0; limit--)
  {
    item = (PPrioItem) cluster_list->Dequeue();
    if (item == NULL)
      break;

    if (item->GetPrio() < avrg_load)
      break;

    mapping[item->GetIDX()] = limit;
  }
  limit++;
     // This case may happen if we have all the clusters of size avrg_load
     // except some of length 0, which have to be mapped later.
  if (limit < 0)
    limit = 0;

     // 4. Use wrap or reflection mapping for the remaining clusters.
     //      Wrap maping:  Virtual processor of cluster j is (j-1) mod q,
     //                    where q is the number of remaining processors
     //                    after Step 3.
  while (item != NULL)
  {
    mapping[item->GetIDX()] = item->GetIDX() % limit;

    item = (PPrioItem) cluster_list->Dequeue();
  }

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PWCMNode node =
      (PWCMNode) dag->GetNode(i);

    node->SetProcID (mapping[node->GetVirtualProcessor()]);
  }
}


void
WCMScheduling::ComputeLoads (void)
{
  load = new double [old_proc->GetNrProc()];
    
  for (int i = 0; i < old_proc->GetNrProc(); i++)
  {
    load[i] = 0;
  }

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PWCMNode node = (PWCMNode) dag->GetNode (i);

    load[node->GetVirtualProcessor()] += node->GetMeanExecTime();
  }
}


double
WCMScheduling::ComputeAvrgLoad (void)
{
  double avrg_load = 0;

  for (int i = 0; i < old_proc->GetNrProc(); i++)
  {
    avrg_load += load[i];
  }

  return avrg_load / proc->GetNrProc();
}

   /**
      Add all the nodes to the heap reprezenting the priority list.
        - ordering criteria: high load => high priority.
      Return value:
        - the priority list.
   */
void
WCMScheduling::MakeClusterList (void)
{
  cluster_list = new Heap();

  for (int i = 0; i < old_proc->GetNrProc(); i++)
  {
    cluster_list->Enqueue (new PrioItem (i, load[i]));
  }
}

