/* $Id: ReadDAG.C,v 1.1 2000/10/19 21:09:36 andi Exp $ */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../dag/DAG.H"
#include "ReadConfig.H"


FILE *dag_file;

void GetTasks (PDAG dag, double node_scale, double edge_scale);
void GetTask (PDAG dag, double node_scale, double edge_scale);
void GetEdge (PDAG dag, int crt_task, double edge_scale);


PDAG
ReadDAG (char *dag_file_name, int sched_alg,
         double node_scale, double edge_scale)
{
  if (DEBUG)
    printf ("Parsing %s:\n", dag_file_name);

     // Open the input DAG file
  dag_file = fopen (dag_file_name, "r");
  if (dag_file == NULL)
  {
    printf ("File not found: %s\n", dag_file_name);
    exit (-1);
  }

     // Fill in the DAG
  PDAG dag = new DAG (sched_alg);

  GetTasks (dag, node_scale, edge_scale);

  return dag;
}

void
GetTasks (PDAG dag, double node_scale, double edge_scale)
{
  int nr_tasks;

//  ReadComments (dag_file);
//  if (fscanf (dag_file, "NODE SCALE: %lf", &node_scale) != 1)
//  {
//    fprintf (stderr, "Error reading DAG file:\n");
//    assert (false);
//  }
//  ReadComments (dag_file);
//  if (fscanf (dag_file, " EDGE SCALE: %lf", &edge_scale) != 1)
//  {
//    fprintf (stderr, "Error reading DAG file:\n");
//    assert (false);
//  }
  ReadComments (dag_file);
  if (fscanf (dag_file, " T: %d", &nr_tasks) != 1)
  {
    fprintf (stderr, "Error reading DAG file:\n");
    assert (false);
  }

  for (int tid = 0; tid < nr_tasks; tid++)
  {
    dag->AddNode (tid, 0);
  }

  for (int tid = 0; tid < nr_tasks; tid++)
  {
    GetTask (dag, node_scale, edge_scale);
  }
}


void
GetTask (PDAG dag, double node_scale, double edge_scale)
{
  int tid, nr_edges;
  sched_flops flops;

  ReadComments (dag_file);

     // Read the task id. and the task number of flops.
  if (fscanf (dag_file, " t%d: %ld", &tid, &flops) != 2)
  {
    fprintf (stderr, "Error reading DAG file:\n");
    assert (false);
  }

  flops = (sched_flops) (flops * node_scale);
  (dag->GetNode(tid))->SetFlops (flops);

     // Read the task's successors
  if (fscanf (dag_file, " s%d:", &nr_edges) != 1)
  {
    fprintf (stderr, "Error reading DAG file:\n");
    assert (false);
  }
  
  for (int i = 0; i < nr_edges; i++)
  {
    GetEdge (dag, tid, edge_scale);
  }
}


void
GetEdge (PDAG dag, int crt_task, double edge_scale)
{
  int next_task;
  sched_msg_size msg_size;

  if (fscanf (dag_file, " %d (%ld)", &next_task, &msg_size) != 2)
  {
    fprintf (stderr, "Error reading DAG file:\n");
    assert (false);
  }
  msg_size = (sched_msg_size) (msg_size * edge_scale);
  dag->AddEdge (crt_task, next_task, msg_size);
}
