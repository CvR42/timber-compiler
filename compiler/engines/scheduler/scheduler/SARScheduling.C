/* $Id: SARScheduling.C,v 1.1 2000/10/19 21:09:46 andi Exp $ */

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

#include <stdio.h>
#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Heap.H"
#include "SARNode.H"
#include "Scheduling.H"
#include "SARScheduling.H"


SARScheduling::SARScheduling (int task_ordering_alg)
    : Scheduling (task_ordering_alg)
{
}


   /**
      Merge clusters.
   */
void
SARScheduling::Schedule (void)
{
  ready_tasks = new Heap();
  proc_free = new sched_time [proc->GetNrProc()];
  cluster_free = new sched_time [old_proc->GetNrProc()];

  MakeNodeList();
  MapClusters();

  Scheduling::OrderTasks();
}


   /**
      Node prio order is the length of the longest path from 
        the current node to an exit node including communication.
      Build a priority list of tasks using the above prio.
   */
void
SARScheduling::MakeNodeList (void)
{
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PSARNode node = (PSARNode) dag->GetNode (i);

    node->ComputeOrderPrio();
  }

  node_list = new Heap();
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PSARNode node = (PSARNode) dag->GetNode (i);

    node_list->Enqueue (node);
  }
}


   /**
      The clusters are mapped when mapping the first task in the
      cluster.  The processor is chosen such that the minimum
      increase in parallel time is achieved.
   */
void
SARScheduling::MapClusters (void)
{
  while (node_list->Head() != NULL)
  {
    PSARNode node = (PSARNode) node_list->Dequeue();
    if (DEBUG)
      printf ("Map task %d\n", node->GetID());

    if (node->GetProcID() != NONE)
      continue;

    sched_time min_time = OrderTasks (node->GetVirtualProcessor(), 0);
    int pid = 0;
    for (int p = 1; p < proc->GetNrProc(); p++)
    {
      sched_time aux_time = OrderTasks (node->GetVirtualProcessor(), p);
      if (aux_time < min_time)
      {
        min_time = aux_time;
        pid = p;
      }
    }

    MapCluster (node, pid);
  }
}


   /**
      Map all the tasks in the same cluster with 'node' to processor 'pid'.
   */
void
SARScheduling::MapCluster (PSARNode node, int pid)
{
  int cluster = node->GetVirtualProcessor();

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PSARNode tmp_node = (PSARNode) dag->GetNode(i);

    if (tmp_node->GetVirtualProcessor() == cluster)
      tmp_node->SetProcID (pid);
  }
}


void
SARScheduling::InitForTaskOrdering (void)
{
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PSARNode node = (PSARNode) dag->GetNode(i);
    node->InitNrUnschedPred();
  }

     // All processors become idle at 0.
  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    proc_free[p] = 0;
  }
     // All clusters become idle at 0.
  for (int p = 0; p < old_proc->GetNrProc(); p++)
  {
    cluster_free[p] = 0;
  }
     // Add ready tasks to the ready task list.
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PSARNode node = (PSARNode) dag->GetNode (i);

    if (node->GetNrInEdges() == 0)
      ready_tasks->Enqueue (node);
  }
}


sched_time
SARScheduling::OrderTasks (int cluster, int pid)
{
     // completion time is the maximum task finish time.
  sched_time completion_time = 0;

  InitForTaskOrdering();

     // Find the start time of the node.
  while ((PSARNode) ready_tasks->Head() != NULL)
  {
    PSARNode node = (PSARNode) ready_tasks->Dequeue();
    sched_time ST = node->ComputeLastMesgArrivalTime (cluster, pid);
    int node_pid = node->GetProcID();
    int node_cluster = node->GetVirtualProcessor();

    if (node_pid != NONE)
    {
      if (proc_free[node_pid] > ST)
        ST = proc_free[node_pid];

      proc_free[node_pid] = ST + node->GetExecTime (node_pid);
    }
    else if (node_cluster == cluster)
    {
      if (proc_free[pid] > ST)
        ST = proc_free[pid];

      proc_free[pid] = ST + node->GetExecTime (pid);
    }
    else
    {
      if (cluster_free[node_cluster] > ST)
        ST = cluster_free[node_cluster];

      cluster_free[node_cluster] = ST + node->GetExecTime (NONE);
    }
    node->SetStartTime (ST);

       // Keep track of the last finishing task.
    if (node->GetFinishTime() > completion_time)
      completion_time = node->GetFinishTime();
        
       // Update ready lists and processor list
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PDAGEdge edge = (PDAGEdge) node->GetOutEdge (i);
      PSARNode succ = (PSARNode) edge->GetToNode();

      if (succ->DecNrUnschedPred() == 0)
        ready_tasks->Enqueue (succ);
    }
  }

  return completion_time;
}


void
SARScheduling::PrintClusters (void)
{
  OrderTasks (NONE, NONE);

  printf ("----------------------\n");

  for (int p = 0; p < old_proc->GetNrProc(); p++)
  {
    printf ("Cluster #%d \n", p);

    for (int nid = 0; nid < dag->GetNrNodes(); nid++)
    {
      PCMrgDAGNode node = (PCMrgDAGNode) dag->GetNode (nid);

      if (node->GetVirtualProcessor() == p)
        printf ("  %d[%5.2f,%5.2f]", node->GetID(), node->GetStartTime(),
                                                    node->GetFinishTime());
    }

    printf ("\n");
  }
  printf ("=============================================\n");

  for (int p = 0; p < proc->GetNrProc(); p++)
  {
    printf ("Proc #%d\n", p);

    for (int nid = 0; nid < dag->GetNrNodes(); nid++)
    {
      PCMrgDAGNode node = (PCMrgDAGNode) dag->GetNode (nid);

      if (node->GetProcID() == p)
        printf ("  %d[%5.2f,%5.2f]", node->GetID(), node->GetStartTime(),
                                                    node->GetFinishTime());
    }

    printf ("\n");
  }
  printf ("----------------------\n");
}
