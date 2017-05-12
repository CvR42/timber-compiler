/* $Id: Processors.H,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#ifndef __PROCESSORS_H
#define __PROCESSORS_H

#include "../dag/NoDupDAGNode.H"
#include "ProcListItem.H"
#include "ProcList.H"


typedef class Processors *PProcessors;

class Processors
{
  public:
    Processors (int v_nr_proc = 0);

    void ClearSched (void);

    PProcListItem AddTask (int proc_id, PNoDupDAGNode node,
                           sched_time start_time);
    PProcListItem AddTask (int proc_id, PNoDupDAGNode node, 
                           sched_time start_time, sched_time finish_time);
    void RemoveTask (int proc_id, PNoDupDAGNode node);

    int IncNrProc ();
    int GetNrProc (void) const;

    void SetProcSpeed (int proc_id, sched_speed speed);
    sched_speed GetProcSpeed (int proc_id) const;
    sched_speed ComputeProcSpeedMean (void);
    sched_speed GetProcSpeedMean (void) const;

    void SetProcPrio (int proc_id, double prio);
    double GetProcPrio (int proc_id) const;

    PProcListItem GetLastTask (int proc_id) const;
    PProcListItem GetFirstTask (int proc_id) const;

    sched_time GetSchedLen (void) const;
    sched_time GetProcReadyTime (int proc_id) const;

    void PrintProcessorsNetwork (void) const;
    void PrintProcessorMapping (void) const;
    void PrintSched_DSC (void) const;
    void PrintScheduleTime (void) const;
    void PrintGanttChart (void) const;

    void PrintCSchedule (void) const;


  private:
     /// Number of available processors
    int nr_proc;
    int max_nr_proc;

     /// Processors speed
    sched_speed *proc_speed;
     /// Processors priority (for heterogeneous computing scheduling)
    double *proc_prio;

     /// Each processor has a list of tasks assigned to it sorted by start time
    PProcList *schedule;        


    sched_time GetProcComputeTime (int pid) const;
    void AllocMoreProc (void);
};

#endif
