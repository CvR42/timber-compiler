/* standard UNIX libaries */
#include <stdio.h>
#include <ctype.h>
#include <tmc.h>
#include <vnusbasetypes.h>

#include "data/Sched.H"
#include "data/arguments.h"

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "io.h"
#include "task.h"


tmstring infilename = tmstringNIL;
tmstring outfilename = tmstringNIL;
tmstring errfilename = tmstringNIL;
tmstring proc_file_name = tmstringNIL;


/* Table of debugging flags plus associated information.

   Table is ended by an entry with flagchar '\0'
 */
dbflag flagtab[] = {
  { 's', &givestat, "statistics" },
  { '\0', (int *)0, "" },
};


int GetSchedAlg (char *alg_name);


/* Print usage message to file 'f'. */
static void usage (FILE *f)
{
  fprintf (f, "\nUsage: scheduler [<flags>] [<infile>]\n");
  fprintf (f, "\nor   : scheduler -h\n\n");
    
  fputs ("<flags> is one or more of the following\n"
         " -d<flags>\tSwitch on the given debugging flags.\n"
         " -e<errfile>\tWrite error messages to the given file instead of stderr.\n"
         " -h\tThis help.\n"
         " -o<outfile>\tWrite output to the given file instead of stdout.\n"
         " -n<network_type>\n"
         "    <network_type> = E  Ethernet (default)\n"
         "    <network_type> = M  Myrinet\n"
         " -f\tOutput_format: l=LOGICAL, n=NUMERIC.\n"
         " -O\tOutput: d=DAG, t=schedule time, r=running time, s=scheduling, "
                      "g=Gantt Chart.\n"
         " -N <NR>  node scale (defefault 1.0)\n"
         " -E <NR>  edge scale (default 1.0)\n"
         " -H  Heterogeneous system. \n"
         " -p <NR>  heap size in pseudo list as fraction of no. of processors"
                  " (default 1.0)\n"
         " -R  Reorder tasks after scheduling \n"
         " -S  Scheduling algorithm (default FCP) \n"
         " -P  Processor configuration file (REQUESTED) \n"
         "\n"
         "Available list scheduling algorithms (<algorithm>): \n"
         "  MCP = Modified Critical Path (Wu, Gajski - Nov 88)\n"
         "  MD  = Mobility-Directed (Wu, Gajski - Nov 88)\n"
         "  ETF = Earliest Task First "
                 "(Hwang, Chow, Anger, Lee - Apr 89)\n"
         "  ERT = Earliest Ready Task "
                 "(Lee, Hwang, Chow, Anger - Jun 88)\n"
         "  DPS = Decisive Path Scheduling "
                 "(Park, Shirazi, Marquis, Choo - Aug 97)\n"
         "  DPS_F = Decisive Path Scheduling (by finish time on proc)\n"
         "  CPM = Critical Path Method "
                 "(Shirazi, Wang, Pathak - Nov 90)\n"
         "  DLS = Dynamic Level Scheduling "
                 "(Sih, Lee - Feb 93)\n"
         "  FCP = Fast Critical Path, prio lists as heaps "
                 "(Andrei - Nov 98)  (DEFAULT)\n"
         "  FLB = Fast Load Balancing, prio lists as heaps "
                 "(Andrei - Jan 99)\n"
         "  FDLS = Fast DLS "
                  "(Andrei - Jun 2000)\n"

         "Available clustering algorithms (<algorithm>): \n"
         "  LC   = Linear Clustering "
                 "(Kim, Browne - 88)\n"
         "  DSC  = Dominant Sequence Clustering "
                 "(Yang, Gerasoulis - 94)\n"

         "Available algorithms (<cluster_merging_algorithm>): \n"
         "  WCM = Wrap Cluster Merging "
                 "(Yang - 93)\n"
         "  SAR = Sarkar's algorithm "
                 "(Sarkar - 89)\n"
         "  LLB = List-Based Load Balancing "
                 "(Radulescu, van Gemund, Lin - 98\n"
         "  GLB = Guided Load Balancing "
                 "(Radulescu, van Gemund - 98)\n"

         "Available algorithms (<task_ordering_algorithm>): \n"
         "  RCP = \n"
         "  LEV = \n"
         "  STE = \n"
         "\n"
         ,
         f
         );
  helpdbflags (f, flagtab);
}

/* Same as fopen, but give error message if file can't be opened */
FILE *ckfopen (const char *nm, const char *acc)
{
  FILE *hnd;

  hnd = fopen (nm, acc);
  if (NULL == hnd)
  {
    (void) strcpy (errarg, nm);
    sys_error (errno);
    exit (1);
  }
  return hnd;
}

/* Similar to freopen, but give error message if file can't be opened.
 * Therefore, file handle need not be returned.
 */
static void ckfreopen (const char *nm, const char *acc, FILE *f)
{
  if (freopen (nm, acc, f) == NULL)
  {
    (void) strcpy (errarg, nm);
    sys_error (errno);
    exit (1);
  }
}

/* Scan command line parameters and options. */
void scanargs_error (int test_condition, char *error_message);

void scanargs (int argc, char *argv[])
{
  int argix, i;
  char op;
  const char *arg;


  DEBUG = false;
  NETWORK_TYPE = EXEC_TIME;
  for (argix = 1; argix < argc; argix++)
  {
    arg = argv[argix];

    if (arg[0] != '-')
    {
      infilename = new_tmstring (argv[argix]);
      break;
    }
    else
    {
      op = arg[1];
      switch (op)
      {
        case 'c':
               code_gen = true;
               break;
        case 'd':
               DEBUG = true;
               setdbflags (&arg[2], flagtab, true);
               break;
        case 'e':
               if (arg[2] != '\0')
               {
                 if (errfilename != tmstringNIL)
                   rfre_tmstring (errfilename);
                 errfilename = new_tmstring (arg+2);
               }
               else
               {
                 if ((argix + 1) >= argc)
                 {
                   error ("missing formalParameter for -e");
                 }
                 else
                 {
                   if (errfilename != tmstringNIL)
                     rfre_tmstring (errfilename);

                   argix++;
                   errfilename = new_tmstring (argv[argix]);
                 }
               }
               break;
        case 'f':
               switch (arg[2])
               {
                 case 'n':
                   output_format |= T2D_NUMERIC;
                   break;
                 case 'l':
                   output_format ^= T2D_NUMERIC;
                   break;
               }
               break;
        case 'h':
               usage (stdout);
               exit(0);
        case 'n':
               switch (arg[2])
               {
                 case 'M':
                   NETWORK_TYPE = MYRINET;
                   break;
                 case 'E':
                   NETWORK_TYPE = ETHERNET;
                   break;
                 case 'T':
                   NETWORK_TYPE = EXEC_TIME;
                   break;
               }
               break;
        case 'p':
               scanargs_error ((argix + 1) < argc,
                               "Missing pseudo heap size value\n");
               pseudo_heap_size = atof (argv[argix+1]);
               argix++;
               break;
        case 'o':
               if (arg[2] != '\0')
               {
                 if (outfilename != tmstringNIL)
                   rfre_tmstring (outfilename);

                 outfilename = new_tmstring (arg+2);
               }
               else
               {
                 if ((argix + 1) >= argc)
                 {
                   error ("missing formalParameter for -o");
                 }
                 else
                 {
                   if (outfilename != tmstringNIL)
                     rfre_tmstring (outfilename);
                   argix++;
                   outfilename = new_tmstring (argv[argix]);
                 }
               }
               break;
        case 'E':
               scanargs_error ((argix + 1) < argc,
                               "Missing edge scale value\n");
               edge_scale = atof (argv[argix+1]);
               argix++;
               scanargs_error (edge_scale > 0,
                               "Negative edge scale value\n");
               break;
        case 'N':
               scanargs_error ((argix + 1) < argc,
                               "Missing node scale value\n");
               node_scale = atof (argv[argix+1]);
               argix++;
               scanargs_error (node_scale > 0,
                               "Negative node scale value\n");
               break;
        case 'H':  /* Heterogeneous System */
               heterogeneous = true;
               break;
        case 'R':
               reorder_scheduled_tasks = true;
               break;
        case 'O':
               for (i = 0; arg[i] != '\0'; i++)
               {
                 switch (arg[i])
                 {
                   case 'd':
                     output_format |= T2D_DAG;
                     break;
                   case 't':
                     output_format |= T2D_SCHEDULE_TIME;
                     break;
                   case 'r':
                     output_format |= T2D_RUNNING_TIME;
                     break;
                   case 'p':
                     output_format |= T2D_PROC_MAPPING;
                     break;
                   case 's':
                     output_format |= T2D_TASK_SCHEDULING;
                     break;
                   case 'g':
                     output_format |= T2D_GANTT_CHART;
                     break;
                 }
               }
               break;
        case 'S':  /* Scheduling Algorithm (default FCP) */
               scanargs_error ((argix + 1) < argc,
                               "Missing scheduling algorithm");
               argix++;
               sched_alg = GetSchedAlg (argv[argix]);

                  /* The main algorithm can't be a cluster merging algorithm */
               if ((sched_alg == SAR) || (sched_alg == GLB) ||
                   (sched_alg == LLB) ||
                   (sched_alg == RCP) || (sched_alg == LEV) ||
                   (sched_alg == STE))
               {
                 usage (stderr);
                 exit (-1);
               }

                  /* If the main algorithm is a clustering algorithm
                     (e.g. DSC, EZ) it must be followed by a cluster
                     merging algorithm. */
               switch (sched_alg)
               {
                 case LC:
                 case DSC:
                   scanargs_error ((argix + 1) < argc,
                                   "Missing cluster merging algorithm");
                   cluster_merging_alg = GetSchedAlg (argv[argix + 1]);
                   switch (cluster_merging_alg)
                   {
                     case LLB:
                       task_ordering_alg = NONE;
                       argix += 1;
                       break;
                     default:
                       scanargs_error ((argix + 2) < argc,
                                       "Missing task order algorithm");
                       task_ordering_alg = GetSchedAlg (argv[argix + 2]);
                       argix += 2;
                       break;
                   }
                   break;
                 default:
                   cluster_merging_alg = NONE;
                   task_ordering_alg = NONE;
               }
               break;
        case 'P':  /* Processor File */
               scanargs_error ((argix + 1) < argc,
                               "Missing processor configuration file\n");
               proc_file_name = new_tmstring (argv[argix + 1]);
               argix++;
               break;
        default:
               fprintf (stderr, "Bad option: -%c\n", op);
               usage (stderr);
               exit (1);
      }
    }
  }

  if (proc_file_name == tmstringNIL)
  {
    fputs ("Missing processor configuration file\n", stderr);
    usage (stderr);
    exit (1);
  }
}

void
scanargs_error (int test_condition, char *error_message)
{
  if (test_condition == false)
  {
    fputs (error_message, stderr);
    usage (stderr);
    exit (-1);
  }
}


void
open_iofiles (void)
{
  if (infilename != tmstringNIL)
    infile = ckfopen (infilename, "r");
  else
    infile = stdin;

  if (outfilename != tmstringNIL)
    outfile = ckfopen (outfilename, "w");
  else
    outfile = stdout;
  
  if (errfilename != tmstringNIL)
    ckfreopen (errfilename, "w", stderr);
}

void
close_iofiles (void)
{
  fclose (infile);
  fclose (outfile);
  fclose (stderr);
}

   /* Read the tm representation of the program from input file. */
void
read_tm_file (vnusprog *p_the_prog)
{
  tm_lineno = 1;
  if (fscan_vnusprog (infile, p_the_prog))
  {
    fprintf (stderr, "vnus: %s(%d): %s\n", infilename, tm_lineno, tm_errmsg);
    exit (-1);
  }
}

   /* Print the tm representation of the resulted program. */
void
print_tm_file (vnusprog *p_the_prog)
{
  TMPRINTSTATE *st;
  int level;

  st = tm_setprint (outfile, 1, 75, 8, 0);
  print_vnusprog (st, *p_the_prog);
  level = tm_endprint (st);
  if (level != 0)
  {
    fprintf (stderr, "Internal error: bracket level = %d\n", level);
    exit (-1);
  }
}

void clean_memory (vnusprog the_prog)
{  
  rfre_vnusprog (the_prog);
  fre_tmstring (infilename);
  fre_tmstring (outfilename);
  fre_tmstring (errfilename);

  flush_tmsymbol();

  if (get_balance_tmadmin() != 0)
  {
    fprintf (stderr, "Tm object allocation not balanced\n");
    givestat = true;
  }

  if (givestat)
  {
    stat_tmadmin (stderr);
    stat_tmstring (stderr);
    report_lognew (stderr);
  }
  flush_lognew();
}


int
GetSchedAlg (char *alg_name)
{
  int alg;

  if (strcmp (alg_name, "HC") == 0)
    alg = HC;
  else if (strcmp (alg_name, "MCP") == 0)
    alg = MCP;
  else if (strcmp (alg_name, "MD") == 0)
    alg = MD;
  else if (strcmp (alg_name, "ETF") == 0)
    alg = ETF;
  else if (strcmp (alg_name, "ERT") == 0)
    alg = ERT;
  else if (strcmp (alg_name, "DPS") == 0)
    alg = DPS;
  else if (strcmp (alg_name, "DPS_F") == 0)
    alg = DPS_F;
  else if (strcmp (alg_name, "CPM") == 0)
    alg = CPM;
  else if (strcmp (alg_name, "DLS") == 0)
    alg = DLS;
  else if (strcmp (alg_name, "HLFET") == 0)
    alg = HLFET;
  else if (strcmp (alg_name, "SCFET") == 0)
    alg = SCFET;
  else if (strcmp (alg_name, "SDFET") == 0)
    alg = SDFET;
  else if (strcmp (alg_name, "MD1") == 0)
    alg = MD1;
  else if (strcmp (alg_name, "HLFET1") == 0)
    alg = HLFET1;
  else if (strcmp (alg_name, "SCFET1") == 0)
    alg = SCFET1;
  else if (strcmp (alg_name, "SDFET1") == 0)
    alg = SDFET1;
  else if (strcmp (alg_name, "MD2") == 0)
    alg = MD2;
  else if (strcmp (alg_name, "HLFET2") == 0)
    alg = HLFET2;
  else if (strcmp (alg_name, "SCFET2") == 0)
    alg = SCFET2;
  else if (strcmp (alg_name, "SDFET2") == 0)
    alg = SDFET2;
  else if (strcmp (alg_name, "MD_F") == 0)
    alg = MD_F;
  else if ((strcmp (alg_name, "HEFT") == 0)||
           (strcmp (alg_name, "HLFET_F") == 0))
    alg = HLFET_F;
  else if (strcmp (alg_name, "SCFET_F") == 0)
    alg = SCFET_F;
  else if (strcmp (alg_name, "SDFET_F") == 0)
    alg = SDFET_F;
  else if (strcmp (alg_name, "FCP") == 0)
    alg = FCP_f;
  else if (strcmp (alg_name, "FCP-f") == 0)
    alg = FCP_f;
  else if (strcmp (alg_name, "FCP-p") == 0)
    alg = FCP_p;
  else if (strcmp (alg_name, "FLB") == 0)
    alg = FLB_f;
  else if (strcmp (alg_name, "FLB-f") == 0)
    alg = FLB_f;
  else if (strcmp (alg_name, "FLB-p") == 0)
    alg = FLB_p;
  else if (strcmp (alg_name, "FLB-f_b") == 0)
    alg = FLB_f_b;
  else if (strcmp (alg_name, "FLB-p_b") == 0)
    alg = FLB_p_b;
  else if (strcmp (alg_name, "FFF-f") == 0)
    alg = FFF_f;
  else if (strcmp (alg_name, "FFF-p") == 0)
    alg = FFF_p;
  else if (strcmp (alg_name, "FDLS-f") == 0)
    alg = FDLS_f;
  else if (strcmp (alg_name, "FDLS-p") == 0)
    alg = FDLS_p;

  else if (strcmp (alg_name, "LC") == 0)
    alg = LC;
  else if (strcmp (alg_name, "DSC") == 0)
    alg = DSC;

  else if (strcmp (alg_name, "LLB") == 0)
    alg = LLB;

  else if (strcmp (alg_name, "WCM") == 0)
    alg = WCM;
  else if (strcmp (alg_name, "SAR") == 0)
    alg = SAR;
  else if (strcmp (alg_name, "GLB") == 0)
    alg = GLB;

  else if (strcmp (alg_name, "RCP") == 0)
    alg = RCP;
  else if (strcmp (alg_name, "LEV") == 0)
    alg = LEV;
  else if (strcmp (alg_name, "STE") == 0)
    alg = STE;
  else
  {
    usage(stderr);
    printf ("\nWrong algorithm: %s\n", alg_name);
    exit (-1);
  }

  if (DEBUG)
    printf ("%s ", alg_name);

  return alg;
}
