// File: l2d.c
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"
#include <unistd.h>
#include <longlong.h>

#ifndef __TCS__
union hack {
    VnusDouble d;
    VnusLong l;
};
#endif

union hack2 {
  float f;
  long  l;
};

VnusDouble Java_java_lang_Double_longBitsToDouble( VnusLong v )
{
  switch(sizeof(VnusDouble))
    {
#ifdef __TCS__
      /* trimedia! has 4-byte doubles... */
    case 4:
      {
	hack2 x;      
	if (     v==signed_longlong(0x7ff00000UL,0x00000000UL))
	  /* positive infinity */
	  {
	    x.l = 0x7f800000LU;
	    return x.f;
	  }
	else if (v==signed_longlong(0xfff00000LU,0x00000000LU))
	  /* negative infinity */
	  {
	    x.l = 0xff800000LU;
	    return x.f;
	  }
	else if (v==signed_longlong(0x7ff80000LU,0x00000000LU))
	  /* Not-A-Number */
	  {
	    x.l = 0x7fc00000LU;
	    return x.f;
	  }
	else if (v==signed_longlong(0x00000000LU,0x00000001LU))
	  {
	    x.l = 0x00000001LU;
	    return x.f;
	  }
	else if (v==signed_longlong(0x7fefffffLU,4294967295LU))
	  /* max. value */
	  {
	    x.l=0x7f7fffffLU;
	    return x.f;
	  }
	printf("%08lX %08lX\n",v.hi,v.lo);
	printf("ERROR: java.lang.Double.longBitsToDouble.\n");
	assert(0); exit(1);
      }
#else
    case 8: /* assume IEEE double precision */
      {
	hack x;      
	x.l = v;
	return x.d;
      }
#endif
    default:
      printf("ERROR: java.lang.Double.longBitsToDouble.\n");
      assert(0); exit(1);
    }

  /* fixes a warning. We shouldn't be here... */

  assert(0);
  exit(1);
  return 0.0;

}
