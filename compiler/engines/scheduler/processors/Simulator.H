/* $Id: Simulator.H,v 1.1 2000/10/19 21:09:39 andi Exp $ */

#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include "../data/Statistics.H"


typedef class Simulator *PSimulator;

class Simulator
{
  public:
    Simulator (void);
    void Run (void);

    void SetNodesDistributionType (int dist_type, double param);
    int GetNodesDistributionType (void) const;
    double GetNodesDistributionParam (void) const;

    void SetEdgesDistributionType (int dist_type, double param);
    int GetEdgesDistributionType (void) const;
    double GetEdgesDistributionParam (void) const;

  private:
    int node_distrib_type;     // Distribution type for nodes.
    double node_param;         // 
    int edge_distrib_type;     // Distribution type for edges.
    double edge_param;         // 

    PStatistics stat;


    void InitForSimulation (void);
};

#endif
 
