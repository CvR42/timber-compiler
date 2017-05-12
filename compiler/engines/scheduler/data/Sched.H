/* $Id: Sched.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __SCHED_H
#define __SCHED_H

#include <stdio.h>
#include <values.h>


#define NONE -1


#define HC                5000
#define OPT               2

#define BEGIN_LIST        10
#define MCP               11
#define MD                12
#define ETF               13
#define ERT               14
#define DLS               15
#define CPM               16
#define WL                17
#define HDFET             18
#define HLFET             19
#define SCFET             20
#define SDFET             21
#define DPS               22
#define DPS_F             23

#define MD1               41
#define HDFET1            42
#define HLFET1            43
#define SCFET1            44
#define SDFET1            45
#define MD2               46
#define HDFET2            47
#define HLFET2            48
#define SCFET2            49
#define SDFET2            50

#define MD_F              146
#define HDFET_F           147
#define HLFET_F           148
#define SCFET_F           149
#define SDFET_F           150

#define FCP_f             61
#define FCP_p             62
#define FLB_f             63
#define FLB_p             64
#define FLB_f_b           65
#define FLB_p_b           66
#define FFF_f             67
#define FFF_p             68
#define FDLS_f            69
#define FDLS_p            70
#define END_LIST          99

#define BEGIN_CLUSTERING  100
#define LC                101
#define DSC               102
#define DSC1              103
#define END_CLUSTRERING   199

#define BEGIN_A_LIM       1000
#define END_A_LIM         1099

#define BEGIN_C_MRG       1100
#define GLB               1101
#define LLB               1202
#define WCM               1301
#define SAR               1302
#define SRT               1400
#define END_C_MRG         1299

#define BEGIN_T_ORD       2100
#define RCP               2101
#define LEV               2102
#define STE               2103
#define END_T_ORD         2199


#define DT_GAUSSIAN  0
#define DT_SIGMA     1


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


typedef long int sched_flops;
typedef long int sched_msg_size;
typedef double sched_speed;

typedef double sched_time;
#define MIN_TIME -MAXDOUBLE
#define MAX_TIME MAXDOUBLE


#define max(a,b) ((a>b)? a: b)

#endif
