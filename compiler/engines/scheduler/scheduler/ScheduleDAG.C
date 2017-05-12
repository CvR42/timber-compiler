/* $Id: ScheduleDAG.C,v 1.2 2000/11/28 13:38:34 frits Exp $ */

#include <sys/time.h>
#include <unistd.h>

#include "../data/arguments.h"
#include "../processors/Simulator.H"
#include "Scheduling.H"

#include "LSAScheduling.H"
#include "LSA1Scheduling.H"
#include "LSA2Scheduling.H"
#include "LSFScheduling.H"
#include "MCPScheduling.H"
#include "ETFScheduling.H"
#include "ERTScheduling.H"
#include "DLSScheduling.H"
#include "DPSScheduling.H"
#include "DPSFScheduling.H"
#include "FCP-f_Scheduling.H"
#include "FCP-p_Scheduling.H"
#include "FLB-f_Scheduling.H"
#include "FLB-p_Scheduling.H"
#include "FFF-f_Scheduling.H"
#include "FFF-p_Scheduling.H"
#include "FDLS-f_Scheduling.H"
#include "FDLS-p_Scheduling.H"

#include "LC_Scheduling.H"
#include "DSCScheduling.H"

#include "LLBScheduling.H"

#include "WCMScheduling.H"
#include "SARScheduling.H"
#include "GLBScheduling.H"
#include "ScheduleDAG.H"


char *nr_sched = "0", *nr_sim = "0";

int heterogeneous = false;
int reorder_scheduled_tasks = false;

int sched_alg = FCP_p;
int cluster_merging_alg = NONE;
int task_ordering_alg = NONE;
double node_scale = 1.0, edge_scale = 1.0;
double pseudo_heap_size = 1;

PScheduling NewScheduler (int sched_alg);
void Schedule (struct timeval *time = NULL);
void PrintTimeVal (const char *mesg, struct timeval *delay);


PScheduling
NewScheduler (int sched_alg)
{
  PScheduling sched = NULL;

  switch (sched_alg)
  {
    case MCP:
      sched = new MCPScheduling ();
      break;
    case ETF:
      sched = new ETFScheduling ();
      break;
    case ERT:
      sched = new ERTScheduling ();
      break;
    case DLS:
      sched = new DLSScheduling ();
      break;
    case DPS:
      sched = new DPSScheduling ();
      break;
    case DPS_F:
      sched = new DPSFScheduling ();
      break;
    case CPM:
    case WL:
      sched = new LLBScheduling ();
      break;
    case MD:
    case HDFET:
    case HLFET:
    case SCFET:
    case SDFET:
      sched = new LSAScheduling ();
      break;
    case MD1:
    case HDFET1:
    case HLFET1:
    case SCFET1:
    case SDFET1:
      sched = new LSA1Scheduling (pseudo_heap_size);
      break;
    case MD2:
    case HDFET2:
    case HLFET2:
    case SCFET2:
    case SDFET2:
      sched = new LSA2Scheduling ();
      break;
    case MD_F:
    case HDFET_F:
    case HLFET_F:
    case SCFET_F:
    case SDFET_F:
      sched = new LSFScheduling ();
      break;
    case FCP_f:
      sched = new FCP_f_Scheduling ();
      break;
    case FCP_p:
      sched = new FCP_p_Scheduling (pseudo_heap_size);
      break;
    case FLB_f:
    case FLB_f_b:
      sched = new FLB_f_Scheduling ();
      break;
    case FLB_p:
    case FLB_p_b:
      sched = new FLB_p_Scheduling (pseudo_heap_size);
      break;
    case FFF_f:
      sched = new FFF_f_Scheduling ();
      break;
    case FFF_p:
      sched = new FFF_p_Scheduling ();
      break;
    case FDLS_f:
      sched = new FDLS_f_Scheduling ();
      break;
    case FDLS_p:
      sched = new FDLS_p_Scheduling (pseudo_heap_size);
      break;

    case LC:
      sched = new LC_Scheduling ();
      break;
    case DSC:
      sched = new DSCScheduling ();
      break;

    case LLB:
      sched = new LLBScheduling ();
      break;

    case WCM:
      sched = new WCMScheduling (task_ordering_alg);
      break;
    case SAR:
      sched = new SARScheduling (task_ordering_alg);
      break;
    case GLB:
      sched = new GLBScheduling (task_ordering_alg);
      break;

    case SRT:
      sched = new Scheduling (RCP);
      break;

    default:
      printf ("\nWrong algorithm: %d\n", sched_alg);
      exit (-1);
  }

  return sched;
}
  

void
ScheduleDAG (void)
{
     // The default network type is Myrinet.  It can be switched
     // with -n option.
  if (heterogeneous == false)
  {
    Schedule();

    if (reorder_scheduled_tasks == true)
    {
      printf ("Reorder scheduled tasks:\n");
      proc->ClearSched();
      sched->Scheduling::Schedule();
      if (DEBUG)
        proc->PrintProcessorMapping();
    }
  }
  else
  {
    struct timeval time1, time2, delay;

       // First: Schedule tasks to the extended set of homogeneous processors
    PProcessors saved_proc = proc;
    int nr_processors = 0;
    for (int p = 0; p < proc->GetNrProc(); p++)
    {
      nr_processors += (int) proc->GetProcSpeed (p);
    }

    proc = new Processors (nr_processors);
    for (int p = 0; p < nr_processors; p++)
    {
      proc->SetProcSpeed (p, 1);
    }
    proc->ComputeProcSpeedMean();

    for (int p = 0, k = 0; p < saved_proc->GetNrProc(); p++)
    {
      for (int i = 0; i < saved_proc->GetProcSpeed (p); i++)
      {
        proc->SetProcPrio (k, saved_proc->GetProcSpeed (p));
        k++;
      }
    }

    Schedule (&delay);

       // Second: Map the tasks to the original set of processors
    if (DEBUG)
      printf ("Map tasks on processors:\n");
    PDAG old_dag = dag;
    dag = old_dag->Copy (SRT);
    
    gettimeofday (&time1, NULL);
       // Get the real processor mappings
    int *map;
    map = new int(proc->GetNrProc());
    for (int p = 0, k = 0; p < saved_proc->GetNrProc(); p++)
    {
      for (int i = 0; i < saved_proc->GetProcSpeed (p); i++)
      {
        map[k] = p;
        k++;
      }
    }

       // Set the processor mappings to the tasks in the DAG
    for (int i = 0; i < dag->GetNrNodes(); i++)
    {
      PNoDupDAGNode old_node = (PNoDupDAGNode) old_dag->GetNode (i);
      PNoDupDAGNode node = (PNoDupDAGNode) dag->GetNode (i);
      int pid = map[old_node->GetProcID()];

      node->SetProcID (pid);
    }

    gettimeofday (&time2, NULL);
    timersub (&time2, &time1, &time1);
    timeradd (&delay, &time1, &delay);


       // Third: Order the maped tasks
    if (DEBUG)
      printf ("Sort mapped tasks:\n");

    delete proc;
    proc = saved_proc;
    sched = NewScheduler (SRT);

    gettimeofday (&time1, NULL);
    sched->Scheduling::Schedule();
    gettimeofday (&time2, NULL);
    timersub (&time2, &time1, &time1);
    timeradd (&delay, &time1, &delay);

    if (DEBUG)
      proc->PrintProcessorMapping();
  }
}


void
Schedule (struct timeval *time)
{
  struct timeval time1, time2, delay;
  PProcessors saved_proc = NULL;

  if (DEBUG)
    printf ("Scheduling:\n");

  old_proc = NULL;
  proc->ComputeProcSpeedMean();

     // If there is a second algorithm (first alg. is clustering)
     // use an unbounded number of processors for scheduling in the
     // first step.
  if (cluster_merging_alg != NONE)
  {
    saved_proc = proc;
    proc = new Processors();
  }

     // First step of scheduling.
  sched = NewScheduler (sched_alg);
  gettimeofday (&time1, NULL);
  sched->Schedule();
  gettimeofday (&time2, NULL);

  timersub (&time2, &time1, &delay);

  if (DEBUG)
    PrintSchedule (&delay);

     // If there is a second algorithm (first alg. is clustering)
     // merge task clusters to the available number of processors.
  if (cluster_merging_alg != NONE)
  {
    if (DEBUG)
      printf ("Scheduling again:\n");


    PDAG old_dag = dag;
    dag = old_dag->Copy (cluster_merging_alg);
    dag->SetVirtualProcInfo (old_dag);

    old_proc = proc;
    proc = saved_proc;


       // Second step of scheduling.
    sched = NewScheduler (cluster_merging_alg);

    gettimeofday (&time1, NULL);
    sched->Schedule();

    gettimeofday (&time2, NULL);
    timersub (&time2, &time1, &time1);
    timeradd (&delay, &time1, &delay);
  }

  if (DEBUG)
      PrintSchedule (&delay);
  if (time != NULL)
    memcpy (time, &delay, sizeof (struct timeval));
}
  

void
PrintSchedule (struct timeval *delay)
{
  if (output_format & T2D_DAG)
    dag->PrintDAG ();
  if (output_format & T2D_DAG_STDS)
    dag->PrintDAG_STDS ();
  if (output_format & T2D_DAG_DSC)
    dag->PrintDAG_DSC ();
  if (output_format & T2D_SCHED_DSC)
    proc->PrintSched_DSC ();

  if (output_format & T2D_SCHEDULE_TIME)
    proc->PrintScheduleTime();

  if (output_format & T2D_RUNNING_TIME)
    PrintTimeVal ("Running time: ", delay);
    
  if (output_format & T2D_TASK_SCHEDULING)
    dag->PrintTaskScheduling ();
    
  if (output_format & T2D_PROC_MAPPING)
    proc->PrintProcessorMapping();

  if (output_format & T2D_GANTT_CHART)
    proc->PrintGanttChart();
}

void
PrintTimeVal (const char *mesg, struct timeval *delay)
{
  struct tm *tt = gmtime (&delay->tv_sec);
  int milisec = delay->tv_usec / 1000;
  int microsec = delay->tv_usec - 1000 * milisec;

  fprintf (outfile, "%s: %02d:%02d:%02d %03d:%03d\n", mesg,
          tt->tm_hour, tt->tm_min, tt->tm_sec, milisec, microsec);
  fprintf (outfile, "----------------------\n");
}


void
Simulate (void)
{
  PSimulator sim = new Simulator();
  int nr_simulations = atoi (nr_sim);

  if (nr_simulations > 0)
    fprintf (outfile, "Nr. simulations: %d\n", nr_simulations);

  for (int i = 0; i < nr_simulations; i++)
  {
    sim->Run();
    proc->PrintProcessorMapping();
  }
}

