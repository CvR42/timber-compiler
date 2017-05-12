/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>

/* the tm support library */
#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "global.h"
#include "tmadmin.h"

#include "io.h"
#include "tm2dag.h"
#include "tm_gen.h"

#include "data/Sched.H"
#include "data/arguments.h"
#include "dag/DAG.H"
#include "scheduler/Scheduling.H"
#include "scheduler/ScheduleDAG.H"
#include "parser/ReadConfig.H"
#include "genC.h"


int main (int argc, char *argv[])
{
  vnusprog the_prog;

     /* Find user options and I/O files.  Open I/O files. */
  scanargs (argc, argv);
  open_iofiles ();

     /* Read the tm representation of the program from input file. */
  read_tm_file (&the_prog);

  dag = new DAG (sched_alg);
  Tm2DAG (the_prog);

  proc = ReadProc (proc_file_name);

  ScheduleDAG();

  tm_gen (the_prog);

  if (code_gen == true)
    print_C_schedule();

  TMPRINTSTATE *tmps;
  tmps = tm_setprint (stdout, 1, 75, 8, 0 );
  print_tm_file (&the_prog);

     /* Cleen up memory. */
  clean_memory (the_prog);
  close_iofiles ();

  return 0;
}

