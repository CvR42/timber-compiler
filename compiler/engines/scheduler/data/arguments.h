/* $Id: arguments.h,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __ARGUMENTS_H
#define __ARGUMENTS_H

#include <stdio.h>


extern int NETWORK_TYPE;
#define ETHERNET  1
#define MYRINET   2
#define EXEC_TIME 3

extern bool DEBUG;

extern char *nr_sched, *nr_sim;

extern int heterogeneous;
extern int reorder_scheduled_tasks;

extern int sched_alg;
extern int cluster_merging_alg;
extern int task_ordering_alg;
extern double node_scale, edge_scale;
extern double pseudo_heap_size;

extern bool code_gen;

extern FILE *infile;
extern FILE *outfile;

/* output print formats */
extern int output_format;
#define T2D_LOGIC            0x0000
#define T2D_NUMERIC          0x1000
#define T2D_DAG              0x0001
#define T2D_SCHEDULE_TIME    0x0002
#define T2D_RUNNING_TIME     0x0004
#define T2D_TASK_SCHEDULING  0x0008
#define T2D_PROC_MAPPING     0x0010
#define T2D_GANTT_CHART      0x0020
#define T2D_DAG_STDS         0x0040
#define T2D_DAG_DSC          0x0080
#define T2D_SCHED_DSC        0x0100
#endif
