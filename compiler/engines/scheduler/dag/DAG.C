/* $Id: DAG.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "../data/Sched.H"
#include "DAGNode.H"
#include "DAGEdge.H"
#include "../dag/CMrgDAGNode.H"
#include "../dag/NoDupDAGNode.H"
#include "../scheduler/MCPNode.H"
#include "../scheduler/MD_Node.H"
#include "../scheduler/DSA_Node.H"
#include "../scheduler/ETFNode.H"
#include "../scheduler/ERTNode.H"
#include "../scheduler/DPSNode.H"
#include "../scheduler/DLSNode.H"
#include "../scheduler/HLFETNode.H"
#include "../scheduler/SCFETNode.H"
#include "../scheduler/SDFETNode.H"
#include "../scheduler/MD1Node.H"
#include "../scheduler/DLS1Node.H"
#include "../scheduler/HLFET1Node.H"
#include "../scheduler/SCFET1Node.H"
#include "../scheduler/SDFET1Node.H"
#include "../scheduler/MD2Node.H"
#include "../scheduler/DLS2Node.H"
#include "../scheduler/HLFET2Node.H"
#include "../scheduler/SCFET2Node.H"
#include "../scheduler/SDFET2Node.H"
#include "../scheduler/FCP-f_Node.H"
#include "../scheduler/FCP-p_Node.H"
#include "../scheduler/FLB-f_Node.H"
#include "../scheduler/FLB-p_Node.H"
#include "../scheduler/FDLS-f_Node.H"
#include "../scheduler/FDLS-p_Node.H"
#include "../scheduler/LC_Node.H"
#include "../scheduler/DSCNode.H"
#include "../scheduler/WCMNode.H"
#include "../scheduler/SARNode.H"
#include "../scheduler/GLBNode.H"
#include "../scheduler/LLBNode.H"
#include "DAG.H"


#define INIT_MAX_NR_NODES  100
#define INIT_MAX_NR_EDGES  250

DAG::DAG (int alg)
{
  int i;

  sched_alg = alg;
    
  max_nr_nodes = INIT_MAX_NR_NODES;
  node = new PDAGNode[INIT_MAX_NR_NODES];
  nr_nodes = 0;
  for (i = 0; i < INIT_MAX_NR_NODES; i++)
    node[i] = NULL;

  max_nr_edges = INIT_MAX_NR_EDGES;
  edge = new PDAGEdge[INIT_MAX_NR_EDGES];
  nr_edges = 0;
  for (i = 0; i < INIT_MAX_NR_EDGES; i++)
    edge[i] = NULL;

//    stat = (Statistics) new StatisticsImplem();
}

DAG::~DAG ()
{
  for (int i = 0; i < nr_nodes; i++)
  {
    assert (node[i] != NULL);
    delete node[i];
  }

  for (int i = 0; i < nr_edges; i++)
  {
    assert (edge[i] != NULL);
    delete edge[i];
  }

  delete node;
  delete edge;
}


PDAGNode
DAG::AddNode (int new_node_id, sched_flops flops)
{
  PDAGNode n = NULL;

  if (new_node_id >= max_nr_nodes)
    AllocMoreNodes();

  if (GetNode (new_node_id) == NULL)
  {
    switch (sched_alg)
    {
      case MCP:
        n = (PDAGNode) new MCPNode (new_node_id, flops);
        break;
      case MD:
        n = (PDAGNode) new MD_Node (new_node_id, flops);
        break;
      case ETF:
        n = (PDAGNode) new ETFNode (new_node_id, flops);
        break;
      case ERT:
        n = (PDAGNode) new ERTNode (new_node_id, flops);
        break;
      case DLS:
        n = (PDAGNode) new DSA_Node (new_node_id, flops);
        break;
      case DPS:
      case DPS_F:
        n = (PDAGNode) new DPSNode (new_node_id, flops);
        break;
      case CPM:
        n = (PDAGNode) new LLBNode (new_node_id, flops);
        break;
      case WL:
        n = (PDAGNode) new LLBNode (new_node_id, flops);
        break;
      case HDFET:
        n = (PDAGNode) new DLSNode (new_node_id, flops);
        break;
      case HLFET:
        n = (PDAGNode) new HLFETNode (new_node_id, flops);
        break;
      case SCFET:
        n = (PDAGNode) new SCFETNode (new_node_id, flops);
        break;
      case SDFET:
        n = (PDAGNode) new SDFETNode (new_node_id, flops);
        break;
      case MD1:
        n = (PDAGNode) new MD1Node (new_node_id, flops);
        break;
      case HDFET1:
        n = (PDAGNode) new DLS1Node (new_node_id, flops);
        break;
      case HLFET1:
        n = (PDAGNode) new HLFET1Node (new_node_id, flops);
        break;
      case SCFET1:
        n = (PDAGNode) new SCFET1Node (new_node_id, flops);
        break;
      case SDFET1:
        n = (PDAGNode) new SDFET1Node (new_node_id, flops);
        break;
      case MD2:
        n = (PDAGNode) new MD2Node (new_node_id, flops);
        break;
      case HDFET2:
        n = (PDAGNode) new DLS2Node (new_node_id, flops);
        break;
      case HLFET2:
        n = (PDAGNode) new HLFET2Node (new_node_id, flops);
        break;
      case SCFET2:
        n = (PDAGNode) new SCFET2Node (new_node_id, flops);
        break;
      case SDFET2:
        n = (PDAGNode) new SDFET2Node (new_node_id, flops);
        break;
      case MD_F:
        n = (PDAGNode) new MD_Node (new_node_id, flops);
        break;
      case HDFET_F:
        n = (PDAGNode) new DLSNode (new_node_id, flops);
        break;
      case HLFET_F:
        n = (PDAGNode) new HLFETNode (new_node_id, flops);
        break;
      case SCFET_F:
        n = (PDAGNode) new SCFETNode (new_node_id, flops);
        break;
      case SDFET_F:
        n = (PDAGNode) new SDFETNode (new_node_id, flops);
        break;
      case FCP_f:
        n = (PDAGNode) new FCP_f_Node (new_node_id, flops);
        break;
      case FCP_p:
        n = (PDAGNode) new FCP_p_Node (new_node_id, flops);
        break;
      case FLB_f:
      case FLB_f_b:
        n = (PDAGNode) new FLB_f_Node (new_node_id, flops);
        break;
      case FLB_p:
      case FLB_p_b:
        n = (PDAGNode) new FLB_p_Node (new_node_id, flops);
        break;
      case FFF_f:
        n = (PDAGNode) new FLB_f_Node (new_node_id, flops);
        break;
      case FDLS_f:
        n = (PDAGNode) new FDLS_f_Node (new_node_id, flops);
        break;
      case FDLS_p:
        n = (PDAGNode) new FDLS_p_Node (new_node_id, flops);
        break;

      case LC:
        n = (PDAGNode) new LC_Node (new_node_id, flops);
        break;
      case DSC:
      case DSC1:
        n = (PDAGNode) new DSCNode (new_node_id, flops);
        break;

      case LLB:
        n = (PDAGNode) new LLBNode (new_node_id, flops);
        break;

      case WCM:
        n = (PDAGNode) new WCMNode (new_node_id, flops);
        break;
      case SAR:
        n = (PDAGNode) new SARNode (new_node_id, flops);
        break;
      case GLB:
        n = (PDAGNode) new GLBNode (new_node_id, flops);
        break;

      case SRT:
        n = (PDAGNode) new CMrgDAGNode (new_node_id, flops);
        break;

      default:
        fprintf (stderr, "dag.DAGImplem.AddNode:  wrong algorithm: %d\n",
                 sched_alg);
        exit (-1);
    }
    node[new_node_id] = n;

    nr_nodes++;
  }

  return n;
}
        
    
PDAGNode
DAG::AddNode (sched_flops flops)
{
  PDAGNode node = NULL;

     // Get the first empty node entry. If necessary memory is allocated.
  int nid = GetFirstEmptyNode ();

     // Effectively add the node
  node = AddNode (nid, flops);
    
  return node;
}


PDAGNode
DAG::GetNode (int node_id) const
{
  return node[node_id];
}


PDAGEdge
DAG::AddEdge (int from_node_id, int to_node_id, sched_msg_size msg_size)
{
  return AddEdge (GetNode (from_node_id), GetNode(to_node_id), msg_size);
}

  
PDAGEdge
DAG::AddEdge (PDAGNode from_node, PDAGNode to_node, sched_msg_size msg_size)
{
  PDAGEdge new_edge;
  int new_edge_id;

  if ((from_node == NULL) || (to_node == NULL))
    return NULL;

  new_edge_id = GetFirstEmptyEdge ();
  new_edge = new DAGEdge (new_edge_id, from_node, to_node, msg_size);

  edge[new_edge_id] = new_edge;
  nr_edges++;
      
  from_node->AddOutEdge (new_edge);
  to_node->AddInEdge (new_edge);

  return new_edge;
}


PDAGEdge
DAG::GetEdge (int edge_id) const
{
  return edge[edge_id];
}
  

PDAGEdge
DAG::GetEdge (PDAGNode node_from, PDAGNode node_to) const
{
  for (unsigned int i = 0; i < node_from->GetNrOutEdges(); i++)
  {
    PDAGEdge edge = (PDAGEdge) node_from->GetOutEdge(i);

    if (node_to == ((PDAGNode) edge->GetToNode()))
      return edge;
  }

  return NULL;
}

PDAGEdge
DAG::GetEdge (int node_from_id, int node_to_id) const
{
  return GetEdge (GetNode(node_from_id), GetNode(node_to_id));
}


int
DAG::GetNrNodes (void) const
{
  return nr_nodes;
}
  
  
int
DAG::GetNrEdges () const
{
  return nr_edges;
}


PDAG
DAG::Copy (int sched_alg)
{
  PDAG new_dag = new DAG (sched_alg);

  for (int n = 0; n < nr_nodes; n++)
  {
    PDAGNode old_node = GetNode(n);
    sched_flops flops = old_node->GetFlops();
    PDAGNode new_node = new_dag->AddNode (n, flops);
    new_node->CopyTaskInfo (old_node);
  }

  for (int e = 0; e < nr_edges; e++)
  {
    PDAGEdge edge = GetEdge (e);

    new_dag->AddEdge (((PDAGNode) edge->GetFromNode())->GetID(),
                     ((PDAGNode) edge->GetToNode())->GetID(),
                     edge->GetMsgSize());
  }
    
  return new_dag;
}


void
DAG::SetVirtualProcInfo (PDAG old_dag)
{
  for (int n = 0; n < nr_nodes; n++)
  {
    PNoDupDAGNode old_node = (PNoDupDAGNode) old_dag->GetNode(n);
    PCMrgDAGNode new_node = (PCMrgDAGNode) GetNode(n);

    int pid = old_node->GetProcID();
    new_node->SetVirtualProcessor (pid);

    sched_time start_time = old_node->GetStartTime();
    new_node->SetOldStartTime (start_time);
    sched_time finish_time = old_node->GetFinishTime();
    new_node->SetOldFinishTime (finish_time);
  }
}



//PDAG
//DAG::Copy (int sched_alg, double node_deviation, double edge_deviation)
//{
//  PDAG new_dag = (DAG) new DAG (sched_alg);
//
//  for (int n = 0; n < nr_nodes; n++)
//  {
//    PDAGNode old_node = GetNode(n);
//    sched_flops flops = old_node->GetFlops();
//    flops = (sched_flops) stat->Gaussian (flops, flops * node_deviation);
//
//    new_dag->AddNode (n, flops);
//  }
//
//  for (int e = 0; e < nr_edges; e++)
//  {
//    PDAGEdge edge = GetEdge (e);
//    sched_msg_size msg_size = edge->GetMsgSize();
//    msg_size = (sched_msg_size) stat->Gaussian (msg_size, msg_size * edge_deviation);
//
//    new_dag->AddEdge (((PDAGNode) edge->GetFromNode())->GetID(),
//                     ((PDAGNode) edge->GetToNode())->GetID(),
//                     msg_size);
//  }
//    
//  return new_dag;
//}


int
DAG::GetFirstEmptyNode (void)
{
  int i;

     // Search for an unsed node entry.
  for (i = 0; i < max_nr_nodes; i++)
  {
    if (GetNode (i) == NULL)
      break;
  }

     // If an unused node entry exists return its index.
  if (i < max_nr_nodes)
    return i;

     // No more unused node entries.
     // Allocate more memory, copy the old node entries in the
     // new allocated entries.
  AllocMoreNodes();

     // The first empty node entry after memory was allocated
     // is the first after 'nr_nodes'.
  return nr_nodes;
}

int
DAG::GetFirstEmptyEdge (void)
{
  int i;

     // Search for an unsed edge entry.
  for (i = 0; i < max_nr_edges; i++)
  {
    if (GetEdge (i) == NULL)
      break;
  }

     // If an unused edge entry exists return its index.
  if (i < max_nr_edges)
    return i;

     // No more unused nedge entries.
     // Allocate more memory, copy the old edge entries in the
     // new allocated entries.
  AllocMoreEdges();

     // The first empty edge entry after memory was allocated
     // is the first after 'nr_edges'.
  return nr_edges;
}

void
DAG::AllocMoreNodes (void)
{
  PDAGNode *aux = node;
  node = new PDAGNode [2 * max_nr_nodes];

  max_nr_nodes *= 2;
  for (int i = 0; i < nr_nodes; i++)
  {
    node[i] = aux[i];
  }

  delete aux;
}

void
DAG::AllocMoreEdges (void)
{
  PDAGEdge *aux = edge;
  edge = new PDAGEdge [2 * max_nr_edges];

  max_nr_edges *= 2;
  for (int i = 0; i < nr_edges; i++)
  {
    edge [i] = aux[i];
  }

  delete aux;
}


void
DAG::PrintDAG (void) const
{
  fprintf (outfile, "T: %d\n\n", nr_nodes);

  for (int nid = 0; nid < nr_nodes; nid++)
  {
    PDAGNode node = GetNode (nid);
    node->Print();
    fprintf (outfile, ": %4ld  ", node->GetFlops());

    fprintf (outfile, "s%d: ", node->GetNrOutEdges());
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PDAGEdge edge = (PDAGEdge) node->GetOutEdge (i);
      PDAGNode succ = (PDAGNode) edge->GetToNode();

      succ->Print();
      fprintf (outfile, "(%ld) ", edge->GetMsgSize());
    }

    fprintf (outfile, "\n");
  }
  fprintf (outfile, "----------------------\n");
}

void
DAG::PrintDAG_STDS (void) const
{
  fprintf (outfile, "%d\n\n", nr_nodes);

  for (int nid = 0; nid < nr_nodes; nid++)
  {
    PDAGNode node = GetNode (nid);
    fprintf (outfile, "%ld  ", node->GetFlops());
    fprintf (outfile, "%d %d   ", node->GetNrInEdges(), node->GetNrOutEdges());

    for (unsigned int i = 0; i < node->GetNrInEdges(); i++)
    {
      PDAGEdge edge = (PDAGEdge) node->GetInEdge (i);
      PDAGNode pred = (PDAGNode) edge->GetFromNode();

      fprintf (outfile, "%d %d  ", pred->GetID()+1, 
               (int) edge->GetCommTime());
    }
    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PDAGEdge edge = (PDAGEdge) node->GetOutEdge (i);
      PDAGNode succ = (PDAGNode) edge->GetToNode();

      fprintf (outfile, "%d %d  ", succ->GetID()+1, 
               (int) edge->GetCommTime());
    }

    fprintf (outfile, "\n");
  }
  fprintf (outfile, "----------------------\n");
}

void
DAG::PrintDAG_DSC (void) const
{
  fprintf (outfile, "%d\n\n", nr_nodes);

  for (int nid = 0; nid < nr_nodes; nid++)
  {
    PNoDupDAGNode node = (PNoDupDAGNode) GetNode (nid);
    fprintf (outfile, "%d %ld %d\n", nid + 1, node->GetFlops(), 
                                     node->GetProcID());

    for (unsigned int i = 0; i < node->GetNrOutEdges(); i++)
    {
      PDAGEdge edge = (PDAGEdge) node->GetOutEdge (i);
      PDAGNode succ = (PDAGNode) edge->GetToNode();

      fprintf (outfile, "%d %d  ", succ->GetID() + 1, 
               (int) edge->GetCommTime());
    }

    fprintf (outfile, "-1\n");
  }
  fprintf (outfile, "----------------------\n");
}


void
DAG::PrintTaskScheduling (void) const
{
  for (int nid = 0; nid < nr_nodes; nid++)
  {
    PNoDupDAGNode node = (PNoDupDAGNode) GetNode (nid);
    node->Print();
    fprintf (outfile, " -> %d (%.2f)\n",
                      node->GetProcID(), node->GetStartTime());
  }
  fprintf (outfile, "----------------------\n");
}
