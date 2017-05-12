/* $Id: Simulator.C,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#include <stdio.h>
#include <unistd.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Statistics.H"
#include "../processors/ProcListItem.H"
#include "../scheduler/Scheduling.H"
#include "Simulator.H"


Simulator::Simulator (void)
{
  stat = new Statistics();
}


void
Simulator::Run (void)
{
  int nr_tasks = dag->GetNrNodes();
  int nr_procs = proc->GetNrProc();
  PProcListItem *nl = new PProcListItem[nr_procs];
  sched_time *time = new sched_time[nr_procs];

  InitForSimulation();

     // The first tasks on each processor start at 0.
  for (int p = 0; p < nr_procs; p++)
  {
    nl[p]= proc->GetFirstTask (p);
    time[p] = 0;
  }

     // 
  while (nr_tasks > 0)
  {
    for (int p = 0; p < nr_procs; p++)
    {
      if (DEBUG)
        printf (" ** P = %d **\n", p);
      for (; nl[p] != NULL; nl[p] = (PProcListItem) nl[p]->GetNextItem())
      {
        if (nl[p]->GetNrUnschedPred() != 0)
        {
          if (DEBUG)
            printf (" BREAK: %d, nr_pred=%d\n",
                    nl[p]->GetCrtNode()->GetID(), nl[p]->GetNrUnschedPred());
          break;
        }

        PDAGNode crt_node = nl[p]->GetCrtNode();
        if (DEBUG)
        {
          printf ("Node: %d,  #%d", crt_node->GetID(), p);
          fflush (stdout);
        }

        for (unsigned int i = 0; i < crt_node->GetNrInEdges(); i++)
        {
          PDAGEdge in_edge = (PDAGEdge) crt_node->GetInEdge (i);
          PNoDupDAGNode prev_node = (PNoDupDAGNode) in_edge->GetFromNode();
          PProcListItem pred_nl = prev_node->GetProcListItem();

          sched_time tmp_time = pred_nl->GetFinishTime() +
            in_edge->GetCommTime (prev_node->GetProcID(), p);

          if (tmp_time > time[p])
            time[p] = tmp_time;
        }

        nl[p]->SetStartTime (time[p]);
        time[p] += crt_node->GetExecTime (p);
        nl[p]->SetFinishTime (time[p]);
        if (DEBUG)
          printf (" [%f,%f]\n", nl[p]->GetStartTime(), nl[p]->GetFinishTime() );

        for (unsigned int i = 0; i < crt_node->GetNrOutEdges(); i++)
        {
          PDAGEdge out_edge = (PDAGEdge) crt_node->GetOutEdge (i);
          PNoDupDAGNode succ_node = (PNoDupDAGNode) out_edge->GetToNode();
          PProcListItem succ_nl = succ_node->GetProcListItem();

          succ_nl->DecNrUnschedPred();
        }

        nr_tasks--;
        if (DEBUG)
          printf ("  %d tasks left\n", nr_tasks);
      }
    }
  }
}

void
Simulator::InitForSimulation (void)
{
  for (int p = 0; p < proc->GetNrProc(); p++)
    for (PProcListItem nl = proc->GetFirstTask (p);
         nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      nl->InitForSimulation();
    }
}


void
Simulator::SetNodesDistributionType (int dist_type, double param)
{
  node_distrib_type = dist_type;

  switch (node_distrib_type)
  {
    case NONE:
           node_param = 0;
           break;
    case DT_GAUSSIAN:
    case DT_SIGMA:
           node_param = param;
           break;
    default:
           fprintf (stderr, "Wrong distribution type\n");
           exit (-1);
  }
}

int
Simulator::GetNodesDistributionType (void) const
{
  return node_distrib_type;
}

double
Simulator::GetNodesDistributionParam (void) const
{
  return node_param;
}


void
Simulator::SetEdgesDistributionType (int dist_type, double param)
{
  edge_distrib_type = dist_type;

  switch (edge_distrib_type)
  {
    case NONE:
           edge_param = 0;
           break;
    case DT_GAUSSIAN:
    case DT_SIGMA:
           edge_param = param;
           break;
    default:
           fprintf (stderr, "Wrong distribution type\n");
           exit (-1);
  }
}

int
Simulator::GetEdgesDistributionType (void) const
{
  return edge_distrib_type;
}

double
Simulator::GetEdgesDistributionParam (void) const
{
  return edge_param;
}
