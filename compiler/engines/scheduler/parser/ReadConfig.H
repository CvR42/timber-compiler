/* $Id: ReadConfig.H,v 1.1 2000/10/19 21:09:36 andi Exp $ */

#ifndef __READCONFIG_H
#define __READCONFIG_H

#include "../dag/DAG.H"
#include "../processors/Processors.H"

PDAG ReadDAG (char *dag_file_name, int sched_alg,
              double node_scale, double edge_scale);
PProcessors ReadProc (char *proc_file_name);

void ReadComments (FILE *file);

#endif
