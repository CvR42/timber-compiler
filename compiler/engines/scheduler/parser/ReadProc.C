/* $Id: ReadProc.C,v 1.1 2000/10/19 21:09:36 andi Exp $ */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../processors/Processors.H"
#include "ReadConfig.H"


FILE *proc_file;


int FindNrProc (void);
void GetProcsSpeed (PProcessors proc);


PProcessors
ReadProc (char *proc_file_name)
{
  if (DEBUG)
    printf ("\nParsing %s:\n", proc_file_name);

     // Open the input proc file.
  proc_file = fopen (proc_file_name, "r");
  if (proc_file == NULL)
  {
    printf ("File not found: %s\n", proc_file_name);
    exit (-1);
  }

     // Fill in proc. 
  int nr_proc = FindNrProc();
  PProcessors proc = new Processors (nr_proc);
  GetProcsSpeed (proc);

  return proc;
}


int
FindNrProc (void)
{
  int nr_proc;

  ReadComments (proc_file);

  if (fscanf (proc_file, "Processors: %d", &nr_proc) != 1)
  {
    fprintf (stderr, "Error reading Proc file:\n");
    assert (false);
  }

  return nr_proc;
}


void
GetProcsSpeed (PProcessors proc)
{
  int first_proc, last_proc, speed;
  int expected_first = 0;

  ReadComments (proc_file);

  while (expected_first < proc->GetNrProc())
  {
    if (fscanf (proc_file, " %d-%d: %d", &first_proc, &last_proc, &speed) != 3)
    {
      fprintf (stderr, "Error reading Proc file:\n");
      assert (false);
    }
    if (first_proc != expected_first)
    {
      fprintf (stderr, "Error reading Proc file:\n");
      assert (false);
    }
    if (last_proc >= proc->GetNrProc())
      last_proc = proc->GetNrProc() - 1;

    for (int p = first_proc; p <= last_proc; p++)
    {
      proc->SetProcSpeed (p, speed);
    }
    expected_first = last_proc + 1;
  }
}


void
ReadComments (FILE *file)
{
  char buf[81];

  while (fscanf (file, " #%c", &buf[0]) == 1)
  {
    if (buf[0] == '\n')
      continue;

    while (fgets (buf, 80, file) != NULL)
    {
      if (buf[strlen(buf)-1] == '\n')
        break;
    }
  }
}
