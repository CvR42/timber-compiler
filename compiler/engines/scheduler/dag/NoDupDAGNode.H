/* $Id: NoDupDAGNode.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __NODUPDAGNODE_H
#define __NODUPDAGNODE_H

#include "../data/HeapItem.H"
#include "../dag/DAGNode.H"
#include "../processors/ProcListItem.H"


typedef class NoDupDAGNode *PNoDupDAGNode;

class NoDupDAGNode :
  public DAGNode
{
  public:
    NoDupDAGNode (int nid, long flops);

    void SetProcID (int pid);
    int GetProcID (void) const;

    void SetProcListItem (PProcListItem nl);
    PProcListItem GetProcListItem () const;

    void SetSeqNrOnProc (int nr);
    int GetSeqNrOnProc (void) const;
  

     /** Compute the start time of this node on a processor
       The start time is determined by 2 factors:
         - task dependencies (a task cannot start its execution until
            it has received all the data it needs)
         - processor utilization (a task cannot start its execution
            if the processor is busy)
       Return value:
         - the start time of this node on 'pid' */
    sched_time ComputeStartTimeOnProc (int pid);
    sched_time GetStartTime (void) const;
    sched_time GetFinishTime (void) const;

    sched_time ComputeSTOnProc (int pid);
    void SetFT (sched_time t);
    sched_time GetFT (void) const;

    void InitNrUnschedPred (void);
    int DecNrUnschedPred (void);
    int GetNrUnschedPred (void) const;

    void SetOldStartTime (sched_time val);
    sched_time GetOldStartTime (void) const;
    void SetOldFinishTime (sched_time val);
    sched_time GetOldFinishTime (void) const;


  protected:
    int proc_id;
    PProcListItem item_in_proc_list;
    int seq_number_on_proc;

    int unscheduled_pred;

    sched_time FT;
    sched_time old_start_time;
    sched_time old_finish_time;
};

#endif

