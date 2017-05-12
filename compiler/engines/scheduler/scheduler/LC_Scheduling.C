/* $Id: LC_Scheduling.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

/*
  LC (Linear Clustering)  by  Kim, Browne - 88

  Characteristics
    - unbounded nr. of processors
    - no duplication
    - O()

  Algorithm description
    - While there are unmapped tasks
      - Find a CP in the task graph.
      - Map the CP to a separate processor.  Remove the mapped tasks.
*/

#include <stdio.h>
#include <stdlib.h>

#include "../data/Sched.H"
#include "../data/Heap.H"
#include "Scheduling.H"
#include "LC_Node.H"
#include "LC_Scheduling.H"


void
LC_Scheduling::Schedule (void)
{
  while (true)
  {
    if (MapNextCluster() == false)
      break;
  }

  AssignTasksToProcessors();
}


   /** Find the next linear cluter, consisitng of the tasks belonging
       to a CP in the unmapped tasks DAG. */
bool
LC_Scheduling::MapNextCluster (void)
{
     // node has the highest b_level
  PLC_Node first_node = ComputeBLevels (false);

  if (first_node == NULL)
    return false;

     // Map the task along th CP to the next processor
  PLC_Node node = first_node;
  int pid = proc->GetNrProc();
  while (node != NULL)
  {
       // Map the task to the current processor.
    node->SetProcID (pid);

       // Find the next unmapped critical task (belonging to the CP).
    PLC_Node next_node = NULL;
    sched_time max_b_level = 0;
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PDAGEdge out_edge = (PDAGEdge) node->GetOutEdge (i);
      PLC_Node succ_node = (PLC_Node) out_edge->GetToNode();

      if (succ_node->GetProcID() != NONE)
        continue;
        
      sched_time this_b_level = succ_node->GetBLevel() +
                                out_edge->GetCommTime();

      if (this_b_level >= max_b_level)
      {
        max_b_level = this_b_level;
        next_node = succ_node;
      }
    }
    node = next_node;
  }

  proc->IncNrProc();

  return true;
}
      

   /** Recomputes b_level for all unmapped nodes.
       Depending on the parameter, the already mapped tasks
       are considered or not.
       Returns the node with the highest b_level. */
PLC_Node
LC_Scheduling::ComputeBLevels (bool consider_mapped_tasks)
{
  sched_time max_b_level = 0;
  PLC_Node b_node = NULL;

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PLC_Node node = (PLC_Node) dag->GetNode(i);
    node->ResetBLevel();
  }

  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PLC_Node node = (PLC_Node) dag->GetNode(i);
    if ((consider_mapped_tasks == false) && (node->GetProcID() != NONE))
      continue;

    sched_time b_level = node->ComputeBLevel (consider_mapped_tasks);

    if (b_level > max_b_level)
    {
      max_b_level = b_level;
      b_node = node;
    }
  }

  return b_node;
}


   /** Make a schedule of the tasks. */
void
LC_Scheduling::AssignTasksToProcessors (void)
{
     // The priority list of tasks sorted by b_level.
  PHeap task_list = new Heap();
  ComputeBLevels (true);
  for (int i = 0; i < dag->GetNrNodes(); i++)
  {
    PLC_Node node = (PLC_Node) dag->GetNode (i);
    task_list->Enqueue (node);
  }

     // Assign the tasks to processors, also computing their start time.
  while (task_list->Head() != NULL)
  {
    PLC_Node node = (PLC_Node) task_list->Dequeue();
    int pid = node->GetProcID();
    sched_time ST = node->ComputeStartTimeOnProc (pid);

    proc->AddTask (pid, node, ST);
  }

  delete task_list;
}
