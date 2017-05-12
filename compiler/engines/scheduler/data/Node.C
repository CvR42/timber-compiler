/* $Id: Node.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdlib.h>

#include "Edge.H"
#include "Node.H"



Node::Node (void)
{
  max_nr_in_edges = 1;
  in_edge = new PEdge[max_nr_in_edges];
  nr_in_edges = 0;

  max_nr_out_edges = 2;
  out_edge = new PEdge[max_nr_out_edges];
  nr_out_edges = 0;
}

Node::~Node ()
{
  delete in_edge;
  delete out_edge;
}


PEdge
Node::GetInEdge (unsigned int in_edge_id) const
{
  if (in_edge_id < nr_in_edges)
    return in_edge[in_edge_id];
  else
    return NULL;
}

PEdge
Node::GetOutEdge (unsigned int out_edge_id) const
{
  if (out_edge_id < nr_out_edges)
    return out_edge[out_edge_id];
  else
    return NULL;
}
  
  
void
Node::AddInEdge (PEdge edge)
{
  if (nr_in_edges >= max_nr_in_edges)
    AllocMoreInEdges();

  in_edge[nr_in_edges] = edge;
  nr_in_edges++;
}

void
Node::AddOutEdge (PEdge edge)
{
  if (nr_out_edges >= max_nr_out_edges)
    AllocMoreOutEdges();

  out_edge[nr_out_edges] = edge;
  nr_out_edges++;
}


unsigned int
Node::GetNrInEdges (void) const
{
  return nr_in_edges;
}

unsigned int
Node::GetNrOutEdges (void) const
{
  return nr_out_edges;
}


PNode
Node::GetPredNode (unsigned int k) const
{
  return in_edge[k]->GetFromNode();
}

PNode
Node::GetSuccNode (unsigned int k) const
{
  return out_edge[k]->GetToNode();
}


PEdge
Node::GetLinkTo (PNode to_node) const
{
  for (unsigned int i = 0; i < nr_out_edges; i++)
  {
    if (out_edge[i]->GetToNode() == to_node)
      return out_edge[i];
  }

  return NULL;
}      
    
PEdge
Node::GetLinkFrom (PNode from_node) const
{
  for (unsigned int i = 0; i < nr_in_edges; i++)
  {
    if (in_edge[i]->GetFromNode() == from_node)
      return in_edge[i];
  }

  return NULL;
}



void
Node::AllocMoreInEdges (void)
{
  PEdge *aux = in_edge;

  in_edge = new PEdge [2 * max_nr_in_edges];
  max_nr_in_edges *= 2;

  for (unsigned int i = 0; i < nr_in_edges; i++)
  {
    in_edge[i] = aux[i];
  }

  delete aux;
}
  
void
Node::AllocMoreOutEdges (void)
{
  PEdge *aux = out_edge;

  out_edge = new PEdge [2 * max_nr_out_edges];
  max_nr_out_edges *= 2;

  for (unsigned int i = 0; i < nr_out_edges; i++)
  {
    out_edge[i] = aux[i];
  }

  delete aux;
}
