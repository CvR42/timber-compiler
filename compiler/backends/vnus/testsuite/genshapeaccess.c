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

// extern void VnusEventNullPointer( void );
// extern void VnusEventArrayBoundViolated( void );
void proc( ShapeD1<VnusChar> * a );
/* ------ end of forward function declarations ------ */
extern ShapeD0<VnusInt> *_a0;
extern ShapeD1<VnusChar> *_a1;
extern ShapeD2<VnusBoolean> *_a2;
extern ShapeD3<VnusDouble> *_a3;
extern ShapeDn<VnusInt,6> *_a6;
/* ------ end of forward variable declarations ------ */
ShapeD0<VnusInt> *_a0;
ShapeD1<VnusChar> *_a1;
ShapeD2<VnusBoolean> *_a2;
ShapeD3<VnusDouble> *_a3;
ShapeDn<VnusInt,6> *_a6;

void proc( ShapeD1<VnusChar> * a )
{
    {
	// Scope: f_scope

	{
	    for( VnusInt _i=0L; _i<12L; ++_i )
	    {
		if( VNUS_UNLIKELY( (a==(ShapeD1<VnusChar> *) NULL) ) )
		{
		    goto notnull0;
		}
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(_i,(*a).GetSize0()) ) )
		{
		    goto bounds0;
		}
		(*a)[_i] = 'a';
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(_i,(*a).GetSize0()) ) )
		{
		    goto bounds0;
		}
		(*a)[_i] = 'b';
	    }
	}
	return;
    }
notnull0:
    VnusEventNullPointer();
    return;
bounds0:
    VnusEventArrayBoundViolated();
}

int main( int argc, char **argv )
{
    vnus_argc = argc;
    vnus_argv = argv;
    _a0 = ShapeD0<VnusInt>::create_filled_shape( (VnusBase::markfntype) NULL, 0L );
    _a1 = ShapeD1<VnusChar>::create_filled_shape( (VnusBase::markfntype) NULL, '?', 10L );
    _a2 = ShapeD2<VnusBoolean>::create_nulled_shape( (VnusBase::markfntype) NULL, 10L, 10L );
    _a3 = ShapeD3<VnusDouble>::create_filled_shape( (VnusBase::markfntype) NULL, 0.0, 10L, 10L, 10L );
    _a6 = ShapeDn<VnusInt,6>::create_nulled_shape( (VnusBase::markfntype) NULL, 2L, 2L, 2L, 2L, 2L, 2L );
    v__writeString( 1L, "a0[]:" );
    v__writeInt( 1L, (*_a0)[0L] );
    v__writeString( 1L, "\n" );
    (*_a0)[0L] = 1L;
    v__writeString( 1L, "a0[]:" );
    v__writeInt( 1L, (*_a0)[0L] );
    v__writeString( 1L, "\n" );
    v__writeString( 1L, "a1[1]:" );
    v__writeChar( 1L, (*_a1)[1L] );
    v__writeString( 1L, "\n" );
    (*_a1)[1L] = 'z';
    v__writeString( 1L, "a1[1]:" );
    v__writeChar( 1L, (*_a1)[1L] );
    v__writeString( 1L, "\n" );
    v__writeString( 1L, "a2[1,2]:" );
    v__writeBoolean( 1L, (*_a2)[12L] );
    v__writeString( 1L, "\n" );
    (*_a2)[12L] = false;
    v__writeString( 1L, "a2[1,2]:" );
    v__writeBoolean( 1L, (*_a2)[12L] );
    v__writeString( 1L, "\n" );
    v__writeString( 1L, "a3[1,2,3]:" );
    v__writeDouble( 1L, (*_a3)[123L] );
    v__writeString( 1L, "\n" );
    (*_a3)[123L] = 21.3;
    v__writeString( 1L, "a3[1,2,3]:" );
    v__writeDouble( 1L, (*_a3)[123L] );
    v__writeString( 1L, "\n" );
    v__writeString( 1L, "a6[1,1,1,1,1,1]:" );
    v__writeInt( 1L, (*_a6)[63L] );
    v__writeString( 1L, "\n" );
    (*_a6)[63L] = (-2L);
    v__writeString( 1L, "a6[1,1,1,1,1,1]:" );
    v__writeInt( 1L, (*_a6)[63L] );
    v__writeString( 1L, "\n" );
    exit( 0 );
    return 0;
}
