/* $Id: DAGEdge.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __DAGEDGE_H
#define __DAGEDGE_H

#include "../data/Sched.H"
#include "../data/Node.H"
#include "../data/Edge.H"
#include "../processors/Processors.H"


typedef class DAGEdge *PDAGEdge;

class DAGEdge :
  public Edge
{
  public:
    DAGEdge (int eid, PNode from_node, PNode to_node, sched_msg_size msg_size);

    int GetID (void) const;

    sched_msg_size GetMsgSize (void) const;
    sched_time GetCommTime (int src_pid, int dest_pid) const;
    sched_time GetCommTime (int network_type) const;
    sched_time GetCommTime (void) const;


  protected:
    int eid;
    sched_msg_size msg_size;
};

#endif
