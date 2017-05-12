/* $Id: DAG.H,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#ifndef __DAG_H
#define __DAG_H

#include <stdio.h>

#include "../data/Sched.H"
#include "DAGNode.H"
#include "DAGEdge.H"


typedef class DAG *PDAG;

class DAG
{
  public:
    DAG (int sched_alg);
    ~DAG ();

    int GetNrNodes (void) const;
    PDAGNode AddNode (sched_flops flops);
    PDAGNode AddNode (int node_id, sched_flops flops);

    int GetNrEdges (void) const;
    PDAGEdge AddEdge (int from_node_id, int to_node_id,
                      sched_msg_size msg_size);
    PDAGEdge AddEdge (PDAGNode node_from, PDAGNode node_to,
                      sched_msg_size msg_size);


    // Get information about nodes
    PDAGNode GetNode (int node_id) const;

    PDAGEdge GetEdge (int edge_id) const;
    PDAGEdge GetEdge (PDAGNode node_from, PDAGNode node_to) const;
    PDAGEdge GetEdge (int node_from_id, int node_to_id) const;

    PDAG Copy (int sched_alg);
//    PDAG Copy (int sched_alg, double node_deviation, double edge_deviation);
    void SetVirtualProcInfo (PDAG old_dag);

    void PrintDAG (void) const;
    void PrintDAG_STDS (void) const;
    void PrintDAG_DSC (void) const;
    void PrintTaskScheduling (void) const;

  private:
    int sched_alg;
  
    int max_nr_nodes;
    int nr_nodes;
    PDAGNode *node;

    int max_nr_edges;
    int nr_edges;
    PDAGEdge *edge;


    int GetFirstEmptyNode (void);
    int GetFirstEmptyEdge (void);
    void AllocMoreNodes (void);
    void AllocMoreEdges (void);
};

#endif
