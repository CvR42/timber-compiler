/* $Id: Processors.C,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../data/Sched.H"
#include "Processors.H"


#define DEFAULT_NR_PROC       64

sched_speed proc_speed_mean = 0;


Processors::Processors (int v_nr_proc)
{
  nr_proc = v_nr_proc;

  if (nr_proc != 0)
    max_nr_proc = nr_proc;
  else
    max_nr_proc = DEFAULT_NR_PROC;

  proc_speed = new sched_speed [max_nr_proc];
  proc_prio = new double [max_nr_proc];
  schedule = new PProcList [max_nr_proc];

  for (int i = 0; i < max_nr_proc; i++)
  {
    proc_speed[i] = 0;
    proc_prio[i] = 0;
    schedule[i] = new ProcList();
  }
}

void
Processors::ClearSched (void)
{
  for (int i = 0; i < max_nr_proc; i++)
  {
    delete schedule[i];
    schedule[i] = new ProcList();
  }
}


int
Processors::IncNrProc (void)
{
     // Get extra memory if needed.
  if (nr_proc == max_nr_proc)
    AllocMoreProc();

     // Set the apropriate values for the new processor.
  proc_speed[nr_proc] = proc_speed_mean;
  proc_prio[nr_proc] = 0;

  nr_proc++;
  return nr_proc - 1;
}


int
Processors::GetNrProc (void) const
{
  return nr_proc;
}


void
Processors::SetProcSpeed (int proc_id, sched_speed speed)
{
  proc_speed[proc_id] = speed;
}

sched_speed
Processors::GetProcSpeed (int proc_id) const
{
  return proc_speed[proc_id];
}

sched_speed
Processors::ComputeProcSpeedMean (void)
{
  proc_speed_mean = 0;
  for (int p = 0; p < nr_proc; p++)
  {
    proc_speed_mean += proc_speed[p];
  }
  proc_speed_mean /= nr_proc;

  return proc_speed_mean;
}

sched_speed
Processors::GetProcSpeedMean (void) const
{
  return proc_speed_mean;
}


void
Processors::SetProcPrio (int proc_id, double prio)
{
  proc_prio[proc_id] = prio;
}

double
Processors::GetProcPrio (int proc_id) const
{
  return proc_prio[proc_id];
}
 

PProcListItem
Processors::AddTask (int proc_id, PNoDupDAGNode node, sched_time start_time)
{
  PProcListItem nl = (PProcListItem) schedule[proc_id]->GetLast();
  int seq_no = 0;

  if (nl != NULL)
  {
    PNoDupDAGNode n = (PNoDupDAGNode) nl->GetCrtNode();
    seq_no = n->GetSeqNrOnProc() + 1;
  }

  node->SetProcID (proc_id);
  node->SetSeqNrOnProc (seq_no);
  node->SetFT (start_time + node->GetExecTime (proc_id));
  
  nl = schedule[proc_id]->AddToBack (node);
  nl->SetStartTime (start_time);
  nl->SetFinishTime (start_time + node->GetExecTime (proc_id));
  node->SetProcListItem (nl);

  return nl;
}

PProcListItem
Processors::AddTask (int proc_id, PNoDupDAGNode node, 
                     sched_time start_time, sched_time finish_time)
{
  PProcListItem nl = (PProcListItem) schedule[proc_id]->GetLast();
  int seq_no = 0;

  if (nl != NULL)
  {
    PNoDupDAGNode node = (PNoDupDAGNode) nl->GetCrtNode();
    seq_no = node->GetSeqNrOnProc() + 1;
  }

  node->SetProcID (proc_id);
  node->SetSeqNrOnProc (seq_no);
  
  nl = schedule[proc_id]->AddToBack (node);
  nl->SetStartTime (start_time);
  nl->SetFinishTime (finish_time);
  node->SetProcListItem (nl);

  return nl;
}
  
void
Processors::RemoveTask (int proc_id, PNoDupDAGNode node)
{
  for (PProcListItem nl = (PProcListItem) schedule[proc_id]->GetLast();
       nl != NULL; nl = (PProcListItem) nl->GetPrevItem())
  {
    if (nl->GetCrtNode() == node)
    {
      schedule[proc_id]->RemoveItem (nl);
      break;
    }
  }

  node->SetProcID (NONE);
}


PProcListItem
Processors::GetLastTask (int proc_id) const
{
  return (PProcListItem) schedule[proc_id]->GetLast();
}

PProcListItem
Processors::GetFirstTask (int proc_id) const
{
  return (PProcListItem) schedule[proc_id]->GetFirst();
}
  

sched_time
Processors::GetSchedLen (void) const
{
  sched_time max_time = 0;

  for (int p = 0; p < GetNrProc(); p++)
  {
    sched_time t = GetProcReadyTime (p);
    if (t > max_time)
      max_time = t;
  }
  return max_time;
}


sched_time
Processors::GetProcReadyTime (int pid) const
{
  assert ((pid >=0) && (pid < nr_proc));
  PProcListItem nl = (PProcListItem) schedule[pid]->GetLast();

  if (nl != NULL) 
    return nl->GetFinishTime();
  else
    return 0;
}


void
Processors::PrintProcessorMapping (void) const
{
  sched_time exec_time = GetSchedLen();

  fprintf (outfile, "Processor mapping:\n");
  
  for (int p = 0; p < GetNrProc(); p++)
  {
    fprintf (outfile, "Proc #%d", p);
  
    for (PProcListItem nl = (PProcListItem) schedule[p]->GetFirst(); 
         nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      PDAGNode node = nl->GetCrtNode();

      fprintf (outfile, "  t%d [%.2f-%.2f]", node->GetID(),
              nl->GetStartTime(), nl->GetFinishTime());
    }
    fprintf (outfile, "\n");
  }
  
  fprintf (outfile, "----------------------\n");
  fprintf (outfile, "Procesor utilization:\n");

  for (int p = 0; p < GetNrProc(); p++)
  {
    sched_time x = (GetProcComputeTime(p) / exec_time) * 100;
    fprintf (outfile, "%d: %.2f\n", p, x);
  }
  
  fprintf (outfile, "----------------------\n");
}

void
Processors::PrintSched_DSC (void) const
{
  fprintf (outfile, "%d\n", GetNrProc());
  
  for (int p = 0; p < GetNrProc(); p++)
  {
    for (PProcListItem nl = (PProcListItem) schedule[p]->GetFirst(); 
         nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      PDAGNode node = nl->GetCrtNode();

      fprintf (outfile, "%d %d %d %d\n", node->GetID() + 1, p,
              (int) nl->GetStartTime(), (int) nl->GetFinishTime());
    }
  }
  fprintf (outfile, "----------------------\n");
}

   
void
Processors::PrintScheduleTime (void) const
{
  fprintf (outfile, "Schedule time: %.2f\n", GetSchedLen());
  fprintf (outfile, "----------------------\n");

}


void
Processors::PrintGanttChart (void) const
{
  sched_time time_unit = 100;

  for (int pid = 0; pid < GetNrProc(); pid++)
  {
    fprintf (outfile, "\n\n          ");
    sched_time crt_time = 0;
    for (PProcListItem nl = (PProcListItem) schedule[pid]->GetFirst(); 
         nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      int nr = (int) ((nl->GetStartTime() - crt_time) / time_unit);
      for (int i = 0; i < nr; i++)
        putc (' ', outfile);

      nr = (int) ((nl->GetFinishTime() - nl->GetStartTime()) / time_unit);
      for (int i = 1; i < (nr/2)-1; i++)
        putc (' ', outfile);
      fprintf (outfile, "%d", nl->GetCrtNode()->GetID());
      for (int i = (nr/2); i < nr; i++)
        putc (' ', outfile);

      crt_time = nl->GetFinishTime();
    }


    fprintf (outfile, "\nProc #%2d: ", pid);

    crt_time = 0;
    for (PProcListItem nl = (PProcListItem) schedule[pid]->GetFirst(); 
         nl != NULL; nl = (PProcListItem) nl->GetNextItem())
    {
      int nr = (int) ((nl->GetStartTime() - crt_time) / time_unit);
      for (int i = 0; i < nr; i++)
        putc (' ', outfile);

      nr = (int) ((nl->GetFinishTime() - nl->GetStartTime()) / time_unit);
      putc ('+', outfile);
      for (int i = 1; i < nr; i++)
        putc ('-', outfile);

      crt_time = nl->GetFinishTime();
    }
  }
  putc ('\n', outfile);
  fprintf (outfile, "----------------------\n");
}


sched_time
Processors::GetProcComputeTime (int pid) const
{
  sched_time time = 0;

  for (PProcListItem nl = (PProcListItem) schedule[pid]->GetFirst(); 
       nl != NULL; nl = (PProcListItem) nl->GetNextItem())
  {
    time += nl->GetCrtNode()->GetExecTime (pid);
  }

  return time;
}


void
Processors::AllocMoreProc (void)
{
  PProcList *aux_s = schedule;
  sched_speed *aux_ps = proc_speed;
  sched_speed *aux_pp = proc_prio;
      
  max_nr_proc = 2 * max_nr_proc;
  schedule = new PProcList [max_nr_proc];
  proc_speed = new sched_speed [max_nr_proc];
  proc_prio = new double [max_nr_proc];

  for (int i = 0; i < nr_proc; i++)
  {
    schedule[i] = aux_s[i];
    proc_speed[i] = aux_ps[i];
    proc_prio[i] = aux_pp[i];
  }
  for (int i = nr_proc; i < max_nr_proc; i++)
  {
    schedule[i] = new ProcList();
  }

  delete aux_s;
  delete aux_ps;
  delete aux_pp;
}



