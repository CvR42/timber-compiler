/* $Id: DAGNode.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../processors/Processors.H"
#include "../scheduler/Scheduling.H"
#include "DAGNode.H"



DAGNode::DAGNode (int v_nid, sched_flops v_flops)
    : Node()
{
  nid = v_nid;

  task_name = NULL;
  nr_card = 0;
  card = NULL;

  flops = v_flops;
  level = NONE;

  mean_exec_time = NONE;
}

DAGNode::~DAGNode ()
{
  free (card);
}

  
int
DAGNode::GetID (void) const
{
  return nid;
}


void
DAGNode::SetTaskInfo (char* name, int nr_card, unsigned int* card)
{
  this->task_name = name;
  this->nr_card = nr_card;
  this->card = card;
}

void
DAGNode::CopyTaskInfo (PDAGNode node)
{
  this->task_name = node->task_name;
  this->nr_card = node->nr_card;
  this->card = node->card;
}

char*
DAGNode::GetTaskName (void) const
{
  return task_name;
}

int
DAGNode::GetTaskNrCard (void) const
{
  return nr_card;
}

unsigned int
DAGNode::GetTaskCard (int i) const
{
  return card[i];
}


void
DAGNode::SetFlops (sched_flops v_flops)
{
  flops = v_flops;
}

sched_flops
DAGNode::GetFlops (void) const
{
  return flops;
}


void
DAGNode::SetMeanExecTime (sched_time mean_proc_speed)
{
  mean_exec_time = flops / mean_proc_speed;
}

sched_time
DAGNode::GetMeanExecTime (void) const
{
  assert (mean_exec_time != NONE);
  return mean_exec_time;
}

   /* The result is in microseconds.
      Processor speed in Mflops/sec.
   */
sched_time
DAGNode::GetExecTime (int proc_id) const
{
  if (proc_id != NONE)
    return flops / proc->GetProcSpeed (proc_id);
  else
    return GetMeanExecTime();
}


int
DAGNode::ComputeLevel (void)
{
  if (level != NONE)
    return level;

  level = 0;
  for (unsigned int i = 0; i < nr_in_edges; i++)
  {
    PDAGNode pred_node = (PDAGNode) GetPredNode (i);
    int tmp_level = pred_node->ComputeLevel();

    if (tmp_level > level)
      level = tmp_level;
  }
  level++;

  return level;
}

int
DAGNode::GetLevel (void) const
{
  return level;
}


void
DAGNode::Print (void) const
{
  if (output_format & T2D_NUMERIC)
  {
    fprintf (outfile, "t%d: ", GetID());
  }
  else
  {
    fprintf (outfile, "%s[", GetTaskName());
    for (int i = 0; i < GetTaskNrCard(); i++)
    {
      if (i == 0)
        fprintf (outfile, "%d", GetTaskCard (i));
      else
        fprintf (outfile, ",%d", GetTaskCard (i));
    }
    fprintf (outfile, "]");
  }
}

