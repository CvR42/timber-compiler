/* $Id: Node.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __NODE_H
#define __NODE_H

typedef class Node *PNode;

#include "Edge.H"


class Node
{
  public:
    Node (void);
    ~Node ();

    unsigned int GetNrInEdges (void) const;
    PEdge GetInEdge (unsigned int in_edge_id) const;

    unsigned int GetNrOutEdges (void) const;
    PEdge GetOutEdge (unsigned int out_edge_id) const;

    void AddInEdge (PEdge edge);
    void AddOutEdge (PEdge edge);

    PNode GetPredNode (unsigned int k) const;
    PNode GetSuccNode (unsigned int k) const;

    PEdge GetLinkTo (PNode to_node) const;
    PEdge GetLinkFrom (PNode from_node) const;

  protected:
    unsigned int max_nr_in_edges;
    unsigned int nr_in_edges;
    unsigned int max_nr_out_edges;
    unsigned int nr_out_edges;

    PEdge *in_edge;
    PEdge *out_edge;

  private:
    void AllocMoreInEdges (void);
    void AllocMoreOutEdges (void);
};

#endif
