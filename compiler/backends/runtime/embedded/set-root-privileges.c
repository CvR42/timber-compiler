
/***************************/
/** set-root-privileges.c **/
/***************************/

#define _BSD_SOURCE

#include <unistd.h>
#include "ensemble-support.h"

void set_root_privileges(int flag)
{
  static int STATE=1;

  if (STATE!=flag)
    {
      setreuid (geteuid (), getuid ());
      STATE=flag;
    }
}
