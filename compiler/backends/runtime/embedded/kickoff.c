
/***************/
/** kickoff.c **/
/***************/

#define _BSD_SOURCE
#define _POSIX_SOURCE

#define VERBOSE 1

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ensemble-support.h"
#include <tmdrvlib.h>
#include <sys/mman.h>

void kickoff(int tmNo, char *imageName, unsigned long bootPage , int procNo,
	     char **baseptr)
{
  pid_t pid;

  fflush(stdout);
  pid = fork();

  assert (pid>=0);

  if (pid==0)
    {
      /* child process. download Trimedia image & run */
      char execstring[1000];
      sprintf(execstring,"tmdl -nocache -tm%d %s --bootpage=0x%08lx --procno=%d",
	      tmNo,imageName,bootPage,procNo);
      if (VERBOSE)
	{
	  printf("execstring: %s\n",execstring);
	  fflush(stdout);
	}
      system(execstring);
      exit(0);
    }
  else
    {
      printf("kicked off %d (pid=%d)\n",tmNo,pid);
    }


  /* now map the entire Trimedia-memory into user-space */

  {
    FILE *pmmap;
    char TMDEVICE[100];
    sprintf(TMDEVICE,"/dev/tm%d",tmNo);

    if (VERBOSE)
      printf("tmdevice: %s\n",TMDEVICE);

    pmmap = openMaintenanceChannel(TMDEVICE, -3);
    assert(pmmap!=0);

    *baseptr = mmap(0,0x800000, PROT_READ|PROT_WRITE,MAP_SHARED,
		    fileno(pmmap),(off_t)0);

    assert((*baseptr)!=0);
    assert((int)(*baseptr) != -1);

  closeMaintenanceChannel(pmmap);
  }
}
