/* $Id: DAGNode.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __DAGNODE_H
#define __DAGNODE_H

#include "../data/Sched.H"
#include "../data/Node.H"


typedef class DAGNode *PDAGNode;

class DAGNode :
  public Node
{
  public:
    DAGNode (int nid, sched_flops flops);
    ~DAGNode ();

    int GetID () const;

    void SetTaskInfo (char* name, int nr_card, unsigned int* card);
    void CopyTaskInfo (PDAGNode node);
    char* GetTaskName (void) const;
    int GetTaskNrCard (void) const;
    unsigned int GetTaskCard (int i) const;

    void SetFlops (sched_flops flops);
    sched_flops GetFlops (void) const;
    void SetMeanExecTime (sched_time mean_proc_time);
    sched_time GetMeanExecTime (void) const;
    sched_time GetExecTime (int proc_id) const;

    int ComputeLevel (void);
    int GetLevel (void) const;

    void Print (void) const;

  protected:
     /// Node identifier
    int nid;

     /// Task's information from the original program
    char* task_name;
    int nr_card;
    unsigned int* card;

     /// Number of flops of the node
    sched_flops flops;
    sched_time mean_exec_time;

    int level;
};

#endif
