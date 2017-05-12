
/***********************/
/** get-locked-page.c **/
/***********************/

#define _SVID_SOURCE

#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ensemble-support.h"

void get_locked_page(void **logical_addr, unsigned long *physical_addr)
     
{
  int shmid;
  int result, memdev;
  unsigned long *logical_address;
  unsigned long page;

  /* allocate a private shared memory page */
  shmid = shmget(IPC_PRIVATE,PAGE_SIZE,0600);
  assert(shmid != -1);

  /* attach to the shared memory page */
  logical_address = shmat(shmid,0,0);
  assert((int)logical_address != -1);

  /* automatically remove the page when no more processes are attached */
  result = shmctl(shmid,IPC_RMID,0);
  assert(result == 0);

  /* lock the page in memory. This requires root privileges */
  set_root_privileges(1);
  result = shmctl(shmid,SHM_LOCK,0);
  set_root_privileges(0);
  assert(result == 0);

  /* open the "physical memory" device */
  set_root_privileges(1);
  memdev = open("/dev/mem",O_RDONLY);
  set_root_privileges(0);
  assert(memdev!=-1);

  /* look for marker */
  *logical_address = 0xdeadbeefLU;
  for(page=0;;page++)
    {
      unsigned long test_value;
      result = lseek(memdev,PAGE_SIZE*page,SEEK_SET);
      assert(result != -1);
      result = read(memdev,&test_value,sizeof(unsigned long));
      assert(result == sizeof(unsigned long));

      /* marker checks out. Now see if changing it in logical space does alter
       * physical space at this address
       */

      if (test_value==*logical_address)
	{
	  *logical_address ^=0xffffffffLU;
	  
	  result = lseek(memdev,PAGE_SIZE*page,SEEK_SET);
	  assert(result != -1);
	  result = read(memdev,&test_value,sizeof(unsigned long));
	  assert(result == sizeof(unsigned long));
	  
	  if (test_value==*logical_address)
	    break;
	  
	  *logical_address ^=0xffffffffLU;
	}
    }
  
  result = close(memdev);
  assert(result==0);

  memset(logical_address,0,PAGE_SIZE);

  if (logical_addr ) *logical_addr  = logical_address;
  if (physical_addr) *physical_addr = PAGE_SIZE*page;
}
