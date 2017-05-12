/* $Id: LC_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __LC_SCHEDULING_H
#define __LC_SCHEDULING_H

#include "../data/Sched.H"
#include "LC_Node.H"

/**
   LC (Linear Clustering)  by  Kim, Browne - 88

   Characteristics
     - unbounded nr. of processors
     - no duplication
     - O()

   Algorithm description
     - While there are unmapped tasks
     - Find a CP in the task graph.
     - Map the CP to a separate cluster.  Remove the mapped tasks.
*/

class LC_Scheduling
  : public Scheduling
{
  public:
    void Schedule (void);


  private:
     /** Recomputes b_level for all unmapped nodes.
       Depending on the parameter, the already mapped tasks
       are considered or not.
       Returns the node with the highest b_level. */
    PLC_Node ComputeBLevels (bool consider_mapped_tasks);

     /** Find the next linear cluter, consisitng of the tasks belonging
         to a CP in the unmapped tasks DAG. */
    bool MapNextCluster (void);

     /** Make a schedule of the tasks. */
    void AssignTasksToProcessors (void);
};

#endif
