/* $Id: GLBScheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

/*
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "GLBClusterItem.H"
#include "GLBNode.H"
#include "GLBScheduling.H"


GLBScheduling::GLBScheduling (int task_ordering_alg)
    : Scheduling (task_ordering_alg)
{
}


   //
   // Merge task clusters.
   //
void
GLBScheduling::Schedule (void)
{
  ComputeNrTaskClusters();
  MakeTaskClusterList();
  MakeProcLists();
    
  MergeTaskClusters();

  OrderTasks();
}


   //
   // Scan all the tasks (already mapped to task clusters) and find
   // how many tasks there are.
   // (Maximum over task cluster id).
   //
void
GLBScheduling::ComputeNrTaskClusters (void)
{
  nr_task_clusters = 0;

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PGLBNode node = (PGLBNode) dag->GetNode (i);

    if (node->GetVirtualProcessor() > nr_task_clusters)
      nr_task_clusters = node->GetVirtualProcessor();
  }

  nr_task_clusters++;
}


   //
   // Sort the task clusters in the increasing order of their start
   // time. Start time of a task cluster is considered the minimum
   // start time of a task belonging to the task cluster.
   //
void
GLBScheduling::MakeTaskClusterList (void)
{
  task_cluster_list = new Heap();

  task_cluster_item = new PGLBClusterItem [nr_task_clusters];
  for (int c = 0; c < nr_task_clusters; c++)
  {
    task_cluster_item[c] = new GLBClusterItem (c, 0);
  }

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PGLBNode node = (PGLBNode) dag->GetNode (i);
    int c = node->GetVirtualProcessor();

    if ((task_cluster_item[c]->GetST() == NONE) ||
        (node->GetOldStartTime() < task_cluster_item[c]->GetST()))
    {
      task_cluster_item[c]->SetST (node->GetOldStartTime());
    }

    task_cluster_item[c]->AddWork (node->GetFlops());
  }

  for (int c = 0; c < nr_task_clusters; c++)
  {
    task_cluster_list->Enqueue (task_cluster_item[c]);
  }
}

    
   //
   // Make processor list.  They will be ordered by their workload.
   // Initially, each processor workload is 0.
   //
void
GLBScheduling::MakeProcLists (void)
{
  proc_list = new Heap;
  proc_item = new PPrioItem [proc->GetNrProc()];
  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    proc_item[p] = new PrioItem (p, 0);
    proc_list->Enqueue (proc_item[p]);
  }
}

  
void
GLBScheduling::MergeTaskClusters (void)
{
     // Map task clusters to processors.
  while (task_cluster_list->Head() != NULL)
  {
    PGLBClusterItem task_cluster = (PGLBClusterItem)
                                   task_cluster_list->Dequeue();
    PPrioItem proc_item = (PPrioItem) proc_list->Head();
    int pid = proc_item->GetIDX();

    task_cluster->SetDestProc (pid);
    proc_item->AddToPrio (- task_cluster->GetWork() / proc->GetProcSpeed(pid));
    proc_list->BalanceHeap (proc_item);
  }

     // Map tasks to procesors (without ordering them).
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PGLBNode node = (PGLBNode) dag->GetNode (i);
    int old_task_cluster = node->GetVirtualProcessor();

    node->SetProcID (task_cluster_item[old_task_cluster]->GetDestProc());
  }
}


void
GLBScheduling::PrintTaskClusters (void)
{
  printf ("----------------------\n");

  for (int p = 0; p < nr_task_clusters; p++)
  {
    printf ("Task Cluster #%d", p);

    for (int nid = 0; nid < dag->GetNrNodes(); nid++)
    {
      PGLBNode node = (PGLBNode) dag->GetNode (nid);

      if (node->GetVirtualProcessor() == p)
          printf ("  %d", node->GetID());
    }

    printf ("\n");
  }
  printf ("=============================================\n");
}
