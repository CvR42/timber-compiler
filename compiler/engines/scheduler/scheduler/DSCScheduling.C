/* $Id: DSCScheduling.C,v 1.1 2000/10/19 21:09:44 andi Exp $ */

/**
  DSC (Dominant Sequence Clustering Algorithm)  by  Yang, Gerasoulis - 1994

  Characteristics
    - unbounded nr. of processors
    - no duplication
    - O((V+E) log(V))
    
  Algorithm description
    - The priorities of a task are an approximation of the length of
      the longest path the task belong to.  It is guaranteed that the
      tasks belonging to the critical path are always prefered.  The
      priorities are calculated at each step, but only for ready tasks
      that can have their priority changed.
    - The cluster chosen is that which determines the minimum start time.
    - If the task to be scheduled does not belong to a critical path
      the cluster is selected such that there will be no delay to the
      start time of a task belonging to a critical path.  This is done
      by tentatively choosing a cluster for a partial ready task
      belonging to a critical path and blocking that cluster as long
      as the critical task is not scheduled.
*/

#include <stdlib.h>
#include <assert.h>

#include "../data/Heap.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "DSCNode.H"
#include "DSCScheduling.H"


DSCScheduling::DSCScheduling (void)
    : Scheduling ()
{
  free_tasks = new Heap();
  partial_free_tasks = new Heap();

   /**
      Calc. botom levels for each task.
      Init. free task list with the entry task.
   */ 
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PDSCNode node = (PDSCNode) dag->GetNode(i);
    node->CalcBLevel();

    if (node->GetNrInEdges() == 0)
      free_tasks->Enqueue (node);
  }
}

DSCScheduling::~DSCScheduling ()
{
  delete free_tasks;
  delete partial_free_tasks;
}


void
DSCScheduling::Schedule (void)
{
  while (true)
  {
    PDSCNode FT_node = (PDSCNode) free_tasks->Dequeue();
    PDSCNode PFT_node = (PDSCNode) partial_free_tasks->Head();

    if (FT_node == NULL)                             // No more tasks
      break;

       // If the PFT exists and has a higher priority than FT
       //  tentatively map it to a cluster.
       // If there is a cluster that reduces its t_level,
       //  mapping of the current FT will be forbidden.
    int DSRW_cluster = NONE;
    if (PFT_node != NULL)
      if (PFT_node->MorePrioThan (FT_node))
      {
        DSRW_cluster = FindCluster (PFT_node, NONE, false);
      }

       // Map the current FT task.  'DSRW_cluster' is forbidden.
    FindCluster (FT_node, DSRW_cluster, true);
      
       // Adjust the FT and PFT priority lists.
    for (unsigned int i = 0; i < FT_node->GetNrOutEdges(); i++) // FT's succ.
    {                                             // could change their status.
      PDAGEdge out_edge = (PDAGEdge) FT_node->GetOutEdge (i);
      PDSCNode succ_node = (PDSCNode) out_edge->GetToNode();

      succ_node->ModifyTLevel (FT_node);               // Modify top_level.

      succ_node->IncNrExaminedParents();               // A new parent examined
      if (succ_node->IsFree())                         //  Task becomes FREE
      {
        partial_free_tasks->RemoveItem (succ_node);   
        free_tasks->Enqueue (succ_node);
        continue;
      }
        
      if (succ_node->IsNewPartialFree())               // Task becomes
      {                                                //  PARTIAL FREE
        partial_free_tasks->Enqueue (succ_node);
      }
      else                                             // Task is already
      {                                                //    PARTIAL FREE
        partial_free_tasks->BalanceHeap (succ_node);
      }
    }
  }
}


   /**
      Find the appropriate cluster for 'node'.
       - 'DSRW_cluster' is the cluster where the free task from the DS with
         a higher priority than 'node' will be mapped.  'DSRW_cluster' is
         forbidden for mapping.  It could be NULL.
       - 'want_mapping' tells if the mapping will be done or not.
      Return value:
       - the cluster identifier if it is an existing one, NONE otherwise
   */
int
DSCScheduling::FindCluster (PDSCNode node, int DSRW_cluster,
                            bool want_mapping)
{
     // If 'node' is an entry task, map it to a new processor.
  if (node->GetNrInEdges() == 0)                     // Entry node
  {
    if (want_mapping)
      proc->AddTask (proc->IncNrProc (), node, node->GetTLevel());
    return NONE;
  }

     // Find the cluster 'node' will be mapped to.
     // If we got NONE, there is no suited cluster => map to a new one.
  int proc_id = GetDestCluster (node, DSRW_cluster);

     // If there is no proper processor to map the task to
     // (proc_id == NONE), map the tsak to a new processor.
  if (proc_id == NONE)
  {
    if (want_mapping)
    {
      proc->AddTask (proc->IncNrProc (), node, node->GetTLevel());
    }
    return NONE;
  }

     // We found the candidate processor for mapping: proc_id.
     // If the proc_id is busy at t_level of 'node', map it to a new proc.
  sched_time level = proc->GetProcReadyTime (proc_id);
  if ((level > start_time) && (level > node->GetTLevel()))
  {
    if (want_mapping)
    {
      proc->AddTask (proc->IncNrProc(), node, node->GetTLevel());
    }
    return NONE;
  }

     // Try mapping some predecessors of 'node' in order to decrease
     // it starting time.
     // Finally map the node to proc_id, if we want that.
  if (want_mapping)
  {
    sched_time tmp_level = TryMappingPred (node, proc_id);

       // level = time proc is idle;  tmp_level = time last msg. arrives
    level = proc->GetProcReadyTime (proc_id);
    if (level > tmp_level)
      tmp_level = level;

    proc->AddTask (proc_id, node, tmp_level);
    node->SetTLevel (tmp_level);
  }
    
  return proc_id;
}


   /**
      Find the cluster, 'node' will be mapped to.
      The candidate cluster for 'node' is the cluster of 'first_node'.
      If that cluster is forbidden, we search the other nodes in the list
      an choose the first available cluster.
      If there is no suited cluster, return NONE.
   */
int
DSCScheduling::GetDestCluster (PDSCNode node, int DSRW_cluster)
{
  PHeap pred_list = MakePredList (node);
  int proc_id = NONE;
  bool tlevel_improved = true;
  sched_time tmp_level = 0;

  while (pred_list->Head() != NULL)
  {
    PPrioItem item = (PPrioItem) pred_list->Dequeue();
    PDSCNode ref_node = (PDSCNode) dag->GetNode (item->GetIDX());
    delete item;
    proc_id = ref_node->GetProcID();
      
    if (proc_id != DSRW_cluster)
    {
      tmp_level = ref_node->GetFinishTime();
      break;
    }

    proc_id = NONE;

       // If we don't map 'node' to the first predecessor in 'pred_list'
       // the 't_level' can't be improved.
    tlevel_improved = false;
  }

     // Update t_level of 'node'.
  if (tlevel_improved)
  {
    PPrioItem item = (PPrioItem) pred_list->Head();

    if (item != NULL)
    {
      if (item->GetPrio() > tmp_level)
        start_time = item->GetPrio();
      else
        start_time = tmp_level;
    }
    else
    {
      start_time = tmp_level;
    }

    delete item;
  }

  delete pred_list;
  return proc_id;
}


   /**
      Try mapping some predecessors of 'node' in order to decrease
      it starting time.
      Return value:
        the time the last message arrives.
   */
//A  Bugs here
sched_time
DSCScheduling::TryMappingPred (PDSCNode node, int proc_id)
{
  sched_time level = proc->GetProcReadyTime (proc_id);
  sched_time tmp_level = 0, new_level = 0;

     // Make the list of candidate tasks that can be moved to
     // the cluster (proc_id) selected for 'node' (i.e. the first
     // tasks in the sorted lists with no more than 1 successor).
  PProcList candidate_tasks = new ProcList();
  MakeCandidateTasks (node, proc_id, candidate_tasks);
    
     // Find the optimum point k in 0..limit-1 such that zeroing
     // edges from predecessors 0..k in sorted list to node
     // t_level (node) is minimized.
  while (true)
  {
       // Find the next pred. mapped on another proc.
    PProcListItem nl = (PProcListItem) candidate_tasks->GetFirst();
    PDSCNode crt = NULL;
    for (; nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      candidate_tasks->RemoveFirst();

      crt = (PDSCNode) nl->GetCrtNode();
      if (crt->GetProcID() != proc_id)
        break;
    }
    if (nl == NULL)                                // No more nodes
      break;

       // The time the message from 'crt' arrives ('crt' not moved).
    PDAGEdge edge = (PDAGEdge) crt->GetLinkTo(node);
    tmp_level = crt->GetFinishTime() +
                edge->GetCommTime (crt->GetProcID(), proc_id);
       // The time 'crt' finishes its execution, if moved to 'proc_id'.
       // If FT >= tmp_level, moving does not improve.
    sched_time ST = GetStartTime (crt, proc_id, level);

    if ((ST + crt->GetExecTime (proc_id)) >= tmp_level)
    {
      if (tmp_level > new_level)
        new_level = tmp_level;
      continue;
    }

       // We can't further improve t_level.
    if (tmp_level < (level + crt->GetExecTime (proc_id)))
    {
      if (tmp_level > new_level)
        new_level = tmp_level;
      break;
    }

       // 'level' is the start time of task 'crt' to 'proc_id'.
    if (ST > level)
      level = ST;
      
       // We can map task 'crt' to 'proc_id'.
    proc->RemoveTask (crt->GetProcID(), crt);
    proc->AddTask (proc_id, crt, level);

       // 'level' will be the time at which 'proc_id' becomes idle.
    level += crt->GetStartTime() + crt->GetExecTime (proc_id);
  }

  delete candidate_tasks;
  return new_level;
}


sched_time
DSCScheduling::GetStartTime (PDSCNode node, int proc_id, sched_time level)
{
  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) node->GetInEdge (i);
    PDSCNode prev_node = (PDSCNode) in_edge->GetFromNode();

    sched_time tmp = prev_node->GetFinishTime() +
                     in_edge->GetCommTime (prev_node->GetProcID(), proc_id);

    if (tmp > level)
      level = tmp;
  }

  return level;
}

   /**
      Make the maximum list of predecessors that have only
      one child (node) taken in the order of the sorting criterion
      mentioned above.
        (Find the maximum integer limit in 1..nr_pred-1 such that
        all the nodes preceding the limit-th in the sorted list
        which are not in the cluster of the first node in list
        have only one child: node).
      Return value:
        The time 'node' would start if the last message is
      comming from the first predecessor in sorted list
      with mode than one successor.
   */
sched_time
DSCScheduling::MakeCandidateTasks (PDSCNode node, int proc_id,
                                   PProcList candidate_tasks)
{
  PHeap pred_list = MakePredList (node);

  while (pred_list->Head() != NULL)
  {
    PPrioItem item = (PPrioItem) pred_list->Dequeue();
    PDSCNode aux_node = (PDSCNode) dag->GetNode (item->GetIDX());
    sched_time prio = item->GetPrio();
    delete item;

    if (aux_node->GetProcID() == proc_id)           // Pred already mapped on
      continue;                                     // the same task cluster.

    if (aux_node->GetNrOutEdges() > 1)              // Pred has >= 1 succ
      return prio;

    assert (((PDSCNode) aux_node->GetOutEdge(0)->GetToNode()) == node);

    candidate_tasks->AddToBack (aux_node);
  }

  return NONE;
}


   /**
      Make the sorted predecessors list of 'node'.  Sort criteria:
       t_level(pred) + node_weight(pred) + edge_weight(node,pred)
   */
PHeap
DSCScheduling::MakePredList (PDSCNode node)
{
  PHeap list = new Heap();

  for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
  {
    PDAGEdge in_edge = (PDAGEdge) node->GetInEdge (i);
    PDSCNode prev_node = (PDSCNode) in_edge->GetFromNode();

    sched_time prio = prev_node->GetTLevel() + prev_node->GetMeanExecTime();
    PPrioItem item = new PrioItem (prev_node->GetID(), prio);

    list->Enqueue (item);
  }

  return list;
}
