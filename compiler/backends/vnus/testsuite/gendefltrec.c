/* This code was generated by the ParTool Vnus Backend. */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <vnuscomplex.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "vnusstio.h"
#include "vnusbaseclass.h"
#include "arrayalloc.h"
#include "vnusservice.h"
#include "shape.h"
#include "vnusrtl.h"
#include "arrayboundviolated.h"
#include "notnullassert.h"

VnusInt numberOfProcessors = 1;
int vnus_argc;
char **vnus_argv;
class record0;

class record0: public VnusBase {
public:
    inline record0()
     
    {}
    inline record0( VnusInt p_f0 ):
	field0(p_f0)
    {}
    VnusInt field0;
};

// extern void VnusEventNullPointer( void );
record0 vnusbug_GiveNorm_0( ShapeD1<VnusDouble> * r_0 );
/* ------ end of forward function declarations ------ */
/* ------ end of forward variable declarations ------ */

record0 vnusbug_GiveNorm_0( ShapeD1<VnusDouble> * r_0 )
{
    {
	// Scope: scope0
	VnusInt breakout0_0;

	if( VNUS_UNLIKELY( (r_0==(ShapeD1<VnusDouble> *) NULL) ) )
	{
	    goto notnull0;
	}
	breakout0_0 = (*r_0).GetSize0();
	return record0( breakout0_0 );
    }
notnull0:
    VnusEventNullPointer();
    return record0( 0L );
}

int main( int argc, char **argv )
{
    vnus_argc = argc;
    vnus_argv = argv;
    (void) vnusbug_GiveNorm_0( ShapeD1<VnusDouble>::create_filled_shape( (VnusBase::markfntype) NULL, 0.0, 23L ) );
    exit( 0 );
    return 0;
}
