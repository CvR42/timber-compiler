#include <stdio.h>

#include "defs.h"
#include "dag/DAG.H"
#include "data/Sched.H"
#include "data/arguments.h"
#include "scheduler/Scheduling.H"
#include "dag_interface.h"
#include "io.h"


int
add_task_to_dag (char* task_name, int nr_card, unsigned int* card, int flops)
{
  sched_flops task_flops = (sched_flops) (flops * node_scale);

  PDAGNode node = dag->AddNode (task_flops);
  node->SetTaskInfo (task_name, nr_card, card);

  return node->GetID();
}

void
add_depend_to_dag (unsigned int from_task_id, unsigned int to_task_id,
                   int msg_size)
{
  sched_msg_size depend_msg_size = (sched_msg_size) msg_size;

  dag->AddEdge (from_task_id, to_task_id, depend_msg_size);
}
