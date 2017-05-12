// File: currentTimeMillis.c
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

VnusLong Java_java_lang_System_currentTimeMillis()
{
#ifdef __GNUC__
    struct timeval t;
    int err = gettimeofday( &t, NULL );
    (void) err;
    VnusLong res = ((VnusLong) t.tv_sec)*1000+(t.tv_usec/1000);
    return res;
#else
    {
      /* if GNU is not available, we provide the following implementation.
	 note that this doesn't actually return the number of millisecs
	 since 1970; instead, it returns msecs since first call.
	 For most purposes (benchmarking!), this is good enough.
      */

      static long first_call = -1;
      long msec = (long)((1000.0*clock())/CLOCKS_PER_SEC);

      if (first_call==-1)
	first_call = msec;

      return VnusLong(msec-first_call);
    }
#endif

}
