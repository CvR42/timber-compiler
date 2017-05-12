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
#include <ctype.h>

VnusInt numberOfProcessors = 1;
int vnus_argc;
char **vnus_argv;
class record2;
class record1;

class record2: public VnusBase {
public:
    inline record2()
     
    {}
    inline record2( VnusInt p_f0, VnusFloat p_f1 ):
	field0(p_f0), field1(p_f1)
    {}
    VnusInt field0;
    VnusFloat field1;
};

class record1: public VnusBase {
public:
    inline record1()
     
    {}
    inline record1( VnusInt p_f0, VnusFloat p_f1, VnusChar p_f2 ):
	field0(p_f0), field1(p_f1), field2(p_f2)
    {}
    VnusInt field0;
    VnusFloat field1;
    VnusChar field2;
};

typedef void proc0();
// extern VnusInt v__open( VnusString v__open_fnm, VnusString v__open_mode );
// extern void v__close( VnusInt v__close_hnd );
// extern void v__writeFloat( VnusInt v__writef_hnd, VnusFloat v__writef_r );
// extern void v__writeComplex( VnusInt v__writec_hnd, VnusComplex v__writec_c );
// extern void v__writeBoolean( VnusInt v__writeb_hnd, VnusBoolean v__writeb_b );
// extern void v__writeInt( VnusInt v__writei_hnd, VnusInt v__writei_n );
// extern void v__writeString( VnusInt v__writes_hnd, VnusString v__writes_s );
void testcasts( void );
void _testscopes( VnusInt _testscopes_f, VnusInt _testscopes_n );
// extern void VnusEventArrayBoundViolated( void );
VnusInt __g_maxloc( ShapeD1<VnusFloat> __l_maxloc_x );
void __g_test_dynamic( VnusInt __l_test_dynamic_f, ShapeD1<VnusInt> * __l_test_dynamic_aout, ShapeD1<VnusInt> * __l_test_dynamic_bout, ShapeD1<VnusInt> * __l_test_dynamic_ain, ShapeD1<VnusInt> * __l_test_dynamic_bin );
void __g_run_test_dynamic( VnusInt __l_run_test_dynamic_f, VnusInt __l_run_test_dynamic_n );
inline void _Empty( void );
void _Print1dShape( VnusInt _Print1dShape_f, ShapeD1<VnusInt> * _Print1dShape_A );
inline VnusInt _inc( VnusInt _inc_i );
VnusInt _dec( VnusInt _dec_i );
void _testcmpint( VnusInt _testcmpint_f, VnusInt _testcmpint_a, VnusInt _testcmpint_b );
/* ------ end of forward function declarations ------ */
extern VnusBase rec0;
extern record1 rec;
extern record2 rec2;
extern record1 *prec;
extern record2 *prec2;
extern VnusFloat _x;
extern VnusInt _i;
extern VnusByte _bt;
extern VnusShort _shrt;
extern VnusLong _l;
extern VnusChar _chr;
extern VnusDouble _d;
extern VnusDouble _inf;
extern VnusDouble _nan;
extern VnusDouble _neginf;
extern VnusFloat _finf;
extern VnusFloat _fnan;
extern VnusFloat _fneginf;
extern void (*_pp)();
extern VnusInt _n;
extern VnusComplex _c;
extern VnusComplex _c2;
extern VnusInt _tpi;
extern VnusInt _tpo;
extern VnusInt _tpio;
extern VnusBoolean _b;
extern VnusBoolean _true;
extern VnusBoolean _false;
extern VnusString _s;
extern VnusInt _f;
extern ShapeD2<VnusInt> *_shp;
extern ShapeD1<VnusInt> *A;
extern ShapeD1<VnusInt> *B;
/* ------ end of forward variable declarations ------ */
VnusBase rec0 = VnusBase(  );
record1 rec = record1( 1L, 1, 'z' );
record2 rec2 = record2( 1L, 1 );
record1 *prec;
record2 *prec2;
VnusFloat _x;
VnusInt _i = 0L;
VnusByte _bt = ((VnusByte) 2);
VnusShort _shrt;
VnusLong _l;
VnusChar _chr = 'X';
VnusDouble _d;
VnusDouble _inf;
VnusDouble _nan;
VnusDouble _neginf;
VnusFloat _finf;
VnusFloat _fnan;
VnusFloat _fneginf;
void (*_pp)();
VnusInt _n;
VnusComplex _c;
VnusComplex _c2;
VnusInt _tpi;
VnusInt _tpo;
VnusInt _tpio;
VnusBoolean _b;
VnusBoolean _true;
VnusBoolean _false;
VnusString _s;
VnusInt _f;
ShapeD2<VnusInt> *_shp;
ShapeD1<VnusInt> *A;
ShapeD1<VnusInt> *B;

void testcasts( void )
{
    // Scope: testcasts
    VnusInt ti;

    ti = (VnusInt) 22.3;
}

void _testscopes( VnusInt _testscopes_f, VnusInt _testscopes_n )
{
    // Scope: renamed_testscopes

    v__writeInt( _testscopes_f, _testscopes_n );
    v__writeString( _testscopes_f, " " );
    {
	// Scope: subscope
	VnusInt _testscopes_n_sub;

	_testscopes_n_sub = 43L;
	v__writeInt( _testscopes_f, _testscopes_n );
	v__writeString( _testscopes_f, " " );
	v__writeInt( _testscopes_f, 43L );
	v__writeString( _testscopes_f, "\n" );
    }
}

VnusInt __g_maxloc( ShapeD1<VnusFloat> __l_maxloc_x )
{
    {
	// Scope: maxloc
	VnusInt __l_maxloc_result;

	__l_maxloc_result = 0L;
	{
	    const VnusInt upperbound0 = (__l_maxloc_x.GetSize0()-1L);

	    for( VnusInt __maxloc_i=0L; __maxloc_i<upperbound0; ++__maxloc_i )
	    {
		if( VNUS_UNLIKELY( (VnusIsBoundViolated((__maxloc_i+1L),__l_maxloc_x.GetSize0())||VnusIsUpperBoundViolated(__maxloc_i,__l_maxloc_x.GetSize0())) ) )
		{
		    goto bounds0;
		}
		if( (__l_maxloc_x[(__maxloc_i+1L)]>__l_maxloc_x[__maxloc_i]) )
		{
		    __l_maxloc_result = (__maxloc_i+1L);
		}
	    }
	}
	return __l_maxloc_result;
    }
bounds0:
    VnusEventArrayBoundViolated();
    return 0L;
}

void __g_test_dynamic( VnusInt __l_test_dynamic_f, ShapeD1<VnusInt> * __l_test_dynamic_aout, ShapeD1<VnusInt> * __l_test_dynamic_bout, ShapeD1<VnusInt> * __l_test_dynamic_ain, ShapeD1<VnusInt> * __l_test_dynamic_bin )
{
    {
	// Scope: test_dynamic
	ShapeD1<VnusInt> *__l_test_dynamic_tmp = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, (*__l_test_dynamic_ain).GetSize0() );
	VnusInt __l_test_dynamic_lim;

	__l_test_dynamic_lim = 10L;
	if( ((*__l_test_dynamic_ain).GetSize0()<10L) )
	{
	    __l_test_dynamic_lim = (*__l_test_dynamic_ain).GetSize0();
	}
	if( ((*__l_test_dynamic_ain).GetSize0()<__l_test_dynamic_lim) )
	{
	    __l_test_dynamic_lim = (*__l_test_dynamic_ain).GetSize0();
	}
	v__writeString( __l_test_dynamic_f, "lim = " );
	v__writeInt( __l_test_dynamic_f, __l_test_dynamic_lim );
	v__writeString( __l_test_dynamic_f, "\n" );
	{
	    const VnusInt upperbound1 = __l_test_dynamic_lim;

	    for( VnusInt __c_i_2=0L; __c_i_2<upperbound1; ++__c_i_2 )
	    {
		if( VNUS_UNLIKELY( ((VnusIsUpperBoundViolated(__c_i_2,(*__l_test_dynamic_tmp).GetSize0())||VnusIsUpperBoundViolated(__c_i_2,(*__l_test_dynamic_ain).GetSize0()))||VnusIsUpperBoundViolated(__c_i_2,(*__l_test_dynamic_bin).GetSize0())) ) )
		{
		    goto bounds1;
		}
		(*__l_test_dynamic_tmp)[__c_i_2] = ((*__l_test_dynamic_ain)[__c_i_2]+(*__l_test_dynamic_bin)[__c_i_2]);
	    }
	}
	{
	    const VnusInt upperbound2 = __l_test_dynamic_lim;

	    for( VnusInt __c_i_3=0L; __c_i_3<upperbound2; ++__c_i_3 )
	    {
		if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(__c_i_3,(*__l_test_dynamic_aout).GetSize0())||VnusIsUpperBoundViolated(__c_i_3,(*__l_test_dynamic_tmp).GetSize0())) ) )
		{
		    goto bounds1;
		}
		(*__l_test_dynamic_aout)[__c_i_3] = (*__l_test_dynamic_tmp)[__c_i_3];
		if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(__c_i_3,(*__l_test_dynamic_bout).GetSize0())||VnusIsUpperBoundViolated(__c_i_3,(*__l_test_dynamic_tmp).GetSize0())) ) )
		{
		    goto bounds1;
		}
		(*__l_test_dynamic_bout)[__c_i_3] = (*__l_test_dynamic_tmp)[__c_i_3];
	    }
	}
	return;
    }
bounds1:
    VnusEventArrayBoundViolated();
}

void __g_run_test_dynamic( VnusInt __l_run_test_dynamic_f, VnusInt __l_run_test_dynamic_n )
{
    {
	// Scope: run_test_dynamic
	ShapeD1<VnusInt> *__l_run_test_dynamic_ain = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 10L );
	ShapeD1<VnusInt> *__l_run_test_dynamic_bin = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, __l_run_test_dynamic_n );
	ShapeD1<VnusInt> *__l_run_test_dynamic_cin = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 10L );
	ShapeD1<VnusInt> *__l_run_test_dynamic_din = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, (__l_run_test_dynamic_n+2L) );
	ShapeD1<VnusInt> *__l_run_test_dynamic_aout = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 10L );
	ShapeD1<VnusInt> *__l_run_test_dynamic_bout = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, __l_run_test_dynamic_n );
	ShapeD1<VnusInt> *__l_run_test_dynamic_cout = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 10L );
	ShapeD1<VnusInt> *__l_run_test_dynamic_dout = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, (__l_run_test_dynamic_n+2L) );
	VnusBoolean __l_run_test_dynamic_bad;
	VnusBoolean __l_run_test_dynamic_allok;

	{
	    for( VnusInt __c_i_8=0L; __c_i_8<10L; ++__c_i_8 )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_8,(*__l_run_test_dynamic_ain).GetSize0()) ) )
		{
		    goto bounds2;
		}
		(*__l_run_test_dynamic_ain)[__c_i_8] = 1L;
	    }
	}
	{
	    const VnusInt upperbound3 = __l_run_test_dynamic_n;

	    for( VnusInt __c_i_9=0L; __c_i_9<upperbound3; ++__c_i_9 )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_9,(*__l_run_test_dynamic_bin).GetSize0()) ) )
		{
		    goto bounds2;
		}
		(*__l_run_test_dynamic_bin)[__c_i_9] = 1L;
	    }
	}
	{
	    for( VnusInt __c_i_a=0L; __c_i_a<10L; ++__c_i_a )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_a,(*__l_run_test_dynamic_cin).GetSize0()) ) )
		{
		    goto bounds2;
		}
		(*__l_run_test_dynamic_cin)[__c_i_a] = 1L;
	    }
	}
	{
	    const VnusInt upperbound4 = (__l_run_test_dynamic_n+2L);

	    for( VnusInt __c_i_b=0L; __c_i_b<upperbound4; ++__c_i_b )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_b,(*__l_run_test_dynamic_din).GetSize0()) ) )
		{
		    goto bounds2;
		}
		(*__l_run_test_dynamic_din)[__c_i_b] = 1L;
	    }
	}
	__g_test_dynamic( __l_run_test_dynamic_f, __l_run_test_dynamic_aout, __l_run_test_dynamic_bout, __l_run_test_dynamic_ain, __l_run_test_dynamic_bin );
	__l_run_test_dynamic_allok = true;
	{
	    for( VnusInt __c_i_c=0L; __c_i_c<10L; ++__c_i_c )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_c,(*__l_run_test_dynamic_aout).GetSize0()) ) )
		{
		    goto bounds2;
		}
		if( ((*__l_run_test_dynamic_aout)[__c_i_c]!=4L) )
		{
		    v__writeString( __l_run_test_dynamic_f, "test_dynamic: bad value for aout\n" );
		    v__writeString( __l_run_test_dynamic_f, "aout[" );
		    v__writeInt( __l_run_test_dynamic_f, __c_i_c );
		    v__writeString( __l_run_test_dynamic_f, "] = " );
		    if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(__c_i_c,(*__l_run_test_dynamic_aout).GetSize0()) ) )
		    {
			goto bounds2;
		    }
		    v__writeInt( __l_run_test_dynamic_f, (*__l_run_test_dynamic_aout)[__c_i_c] );
		    v__writeString( __l_run_test_dynamic_f, "\n" );
		    __l_run_test_dynamic_allok = false;
		}
	    }
	}
	__l_run_test_dynamic_bad = false;
	{
	    for( VnusInt __c_i_d=0L; __c_i_d<10L; ++__c_i_d )
	    {
		__l_run_test_dynamic_bad = (__l_run_test_dynamic_bad||((*__l_run_test_dynamic_bout)[VnusUpperCheckBound(__c_i_d,(*__l_run_test_dynamic_bout).GetSize0())]!=(*__l_run_test_dynamic_aout)[VnusUpperCheckBound(__c_i_d,10L)]));
	    }
	}
	if( __l_run_test_dynamic_bad )
	{
	    v__writeString( __l_run_test_dynamic_f, "test_dynamic: bad value for bout\n" );
	    __l_run_test_dynamic_allok = false;
	}
	__l_run_test_dynamic_bad = false;
	{
	    for( VnusInt __c_i_e=0L; __c_i_e<10L; ++__c_i_e )
	    {
		__l_run_test_dynamic_bad = (__l_run_test_dynamic_bad||((*__l_run_test_dynamic_cout)[VnusUpperCheckBound(__c_i_e,10L)]!=(*__l_run_test_dynamic_bout)[VnusUpperCheckBound(__c_i_e,(*__l_run_test_dynamic_bout).GetSize0())]));
	    }
	}
	if( __l_run_test_dynamic_bad )
	{
	    v__writeString( __l_run_test_dynamic_f, "test_dynamic: bad value for cout\n" );
	    __l_run_test_dynamic_allok = false;
	}
	__l_run_test_dynamic_bad = false;
	{
	    for( VnusInt __c_i_f=0L; __c_i_f<10L; ++__c_i_f )
	    {
		if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(__c_i_f,(*__l_run_test_dynamic_dout).GetSize0())||VnusIsUpperBoundViolated(__c_i_f,10L)) ) )
		{
		    goto bounds2;
		}
		__l_run_test_dynamic_bad = __l_run_test_dynamic_bad||((*__l_run_test_dynamic_dout)[__c_i_f]!=(*__l_run_test_dynamic_cout)[__c_i_f]);
	    }
	}
	if( __l_run_test_dynamic_bad )
	{
	    v__writeString( __l_run_test_dynamic_f, "test_dynamic: bad value for dout\n" );
	    __l_run_test_dynamic_allok = false;
	}
	if( __l_run_test_dynamic_allok )
	{
	    v__writeString( __l_run_test_dynamic_f, "test_dynamic: everything ok\n" );
	}
	return;
    }
bounds2:
    VnusEventArrayBoundViolated();
}

inline void _Empty( void )
{
}

void _Print1dShape( VnusInt _Print1dShape_f, ShapeD1<VnusInt> * _Print1dShape_A )
{
    {
	// Scope: _Print1dShape

	{
	    const VnusInt upperbound5 = (*_Print1dShape_A).GetSize0();

	    for( VnusInt _Print1dShape_i=0L; _Print1dShape_i<upperbound5; ++_Print1dShape_i )
	    {
		if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(_Print1dShape_i,(*_Print1dShape_A).GetSize0()) ) )
		{
		    goto bounds3;
		}
		v__writeInt( _Print1dShape_f, (*_Print1dShape_A)[_Print1dShape_i] );
		v__writeString( _Print1dShape_f, " " );
	    }
	}
	v__writeString( _Print1dShape_f, "\n" );
	return;
    }
bounds3:
    VnusEventArrayBoundViolated();
}

inline VnusInt _inc( VnusInt _inc_i )
{
    // Scope: _inc

    return (_inc_i+1L);
}

VnusInt _dec( VnusInt _dec_i )
{
    // Scope: _dec

    return (_dec_i-1L);
}

void _testcmpint( VnusInt _testcmpint_f, VnusInt _testcmpint_a, VnusInt _testcmpint_b )
{
    // Scope: _testcmpint

    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, "=" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a==_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, "<>" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a!=_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, "<" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a<_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, "<=" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a<=_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, ">" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a>_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
    v__writeInt( _testcmpint_f, _testcmpint_a );
    v__writeString( _testcmpint_f, ">=" );
    v__writeInt( _testcmpint_f, _testcmpint_b );
    if( (_testcmpint_a>=_testcmpint_b) )
    {
	v__writeString( _testcmpint_f, " yes\n" );
    }
    else
    {
	v__writeString( _testcmpint_f, " no\n" );
    }
}

int main( int argc, char **argv )
{
    vnus_argc = argc;
    vnus_argv = argv;
    _shp = ShapeD2<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 5L, 8L );
    A = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 20L );
    B = ShapeD1<VnusInt>::create_nulled_shape( (VnusBase::markfntype) NULL, 20L );
    _f = (VnusInt) v__open( "runout", "w" );
    v__writeString( _f, "Hello world\n" );
    v__writeString( _f, "Hello world" );
    v__writeString( _f, "\n" );
    v__writeString( _f, "\n" );
    _bt = ((VnusByte) 12);
    _shrt = ((VnusShort) 23);
    _l = (-12432342LL);
    _d = (-1.234e-122);
    _inf = VnusLongBitsToDouble(9218868437227405312LL);
    _neginf = VnusLongBitsToDouble(-4503599627370496LL);
    _nan = VnusLongBitsToDouble(9221120237041090560LL);
    _finf = VnusIntBitsToFloat(2139095040L);
    _fneginf = VnusIntBitsToFloat(-8388608L);
    _fnan = VnusIntBitsToFloat(2143289344L);
    _chr = '?';
    _false = false;
    _true = true;
    _b = false;
    _b = true;
    _b = (false|true);
    _b = (true&&_b);
    _i = 2L;
    _i = 48L;
    _i = 53L;
    _i = 265L;
    _i = 22L;
    _i = (-22L);
    _i = (-22L);
    _i = _inc( _inc( (-22L) ) );
    _i = _dec( _i );
    _n = 7L;
    _n = 7L;
    _n = 14L;
    _n = 9L;
    _n = 108L;
    _n = 8L;
    _x = 0.0;
    _x = 2.1;
    v__writeString( _f, "initial x: " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = (-_x);
    v__writeString( _f, "x after ~: " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = _x;
    v__writeString( _f, "x after unary +: " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = (0.7+_x);
    v__writeString( _f, "after (0.7+x): " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = (_x-500);
    v__writeString( _f, "after (x-5e2): " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = (_x*0.12);
    v__writeString( _f, "after (x*0.12): " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = (_x/0.13);
    v__writeString( _f, "after (x/0.13): " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _x = fmod(_x,0.013);
    v__writeString( _f, "after (x mod 0.013): " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    _s = "test string";
    _c = VnusComplex( 2, 3 );
    _c2 = VnusComplex( 5, 12 );
    _c = (-_c);
    (void) _inc( _inc( _i ) );
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    _c = _c;
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    _c = (_c+_c2);
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    _c = (_c*_c2);
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    _c = (_c-_c2);
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    _c = (_c/_c2);
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    v__writeString( _f, "n: " );
    v__writeInt( _f, 8L );
    v__writeString( _f, "\n" );
    v__writeString( _f, "i: " );
    v__writeInt( _f, _i );
    v__writeString( _f, "\n" );
    v__writeString( _f, "b: " );
    v__writeBoolean( _f, _b );
    v__writeString( _f, "\n" );
    v__writeString( _f, "x: " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    v__writeString( _f, "s: " );
    v__writeString( _f, _s );
    v__writeString( _f, "\n" );
    v__writeString( _f, "c: " );
    v__writeComplex( _f, _c );
    v__writeString( _f, "\n" );
    v__writeString( _f, "\n" );
    v__writeString( _f, "n: " );
    v__writeInt( _f, 8L );
    v__writeString( _f, "\n" );
    v__writeString( _f, "i: " );
    v__writeInt( _f, _i );
    v__writeString( _f, "\n" );
    v__writeString( _f, "b: " );
    v__writeBoolean( _f, _b );
    v__writeString( _f, "\n" );
    v__writeString( _f, "x: " );
    v__writeFloat( _f, _x );
    v__writeString( _f, "\n" );
    v__writeString( _f, "s: " );
    v__writeString( _f, _s );
    v__writeString( _f, "\n" );
    _i = 14L;
    while( (_i>0L) )
    {
	v__writeString( _f, "i = " );
	v__writeInt( _f, _i );
	v__writeString( _f, "\n" );
	_i = (_i-1L);
    }
    do
    {
	v__writeString( _f, "i = " );
	v__writeInt( _f, _i );
	v__writeString( _f, "\n" );
	_i -= 1L;
    }
    while( (_i>0L) );
    {
	for( VnusInt _card_i=0L; _card_i<3L; ++_card_i )
	{
	    for( VnusInt _card_j=0L; _card_j<4L; ++_card_j )
	    {
		v__writeString( _f, "i = " );
		v__writeInt( _f, _card_i );
		v__writeString( _f, ", " );
		v__writeString( _f, "j = " );
		v__writeInt( _f, _card_j );
		v__writeString( _f, "\n" );
	    }
	}
    }
    _testcmpint( _f, 1L, 2L );
    _testcmpint( _f, 1L, 1L );
    _testcmpint( _f, 2L, 1L );
    _testcmpint( _f, 223L, 1L );
    {
	for( VnusInt _card_i=0L; _card_i<5L; ++_card_i )
	{
	    for( VnusInt _card_j=0L; _card_j<8L; ++_card_j )
	    {
		if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(_card_i,(*_shp).GetSize0())||VnusIsUpperBoundViolated(_card_j,(*_shp).GetSize1())) ) )
		{
		    VnusEventArrayBoundViolated();
		}
		(*_shp)[(((*_shp).GetSize1()*_card_i)+_card_j)] = (_card_i+_card_j);
	    }
	}
    }
    {
	for( VnusInt _card_i=0L; _card_i<5L; ++_card_i )
	{
	    for( VnusInt _card_j=0L; _card_j<8L; ++_card_j )
	    {
		v__writeString( _f, "shp[" );
		v__writeInt( _f, _card_i );
		v__writeString( _f, "," );
		v__writeInt( _f, _card_j );
		v__writeString( _f, "] = " );
		if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(_card_i,(*_shp).GetSize0())||VnusIsUpperBoundViolated(_card_j,(*_shp).GetSize1())) ) )
		{
		    VnusEventArrayBoundViolated();
		}
		v__writeInt( _f, (*_shp)[(((*_shp).GetSize1()*_card_i)+_card_j)] );
		v__writeString( _f, "\n" );
	    }
	}
    }
    _n = 0L;
    v__writeString( _f, "A := 66;\n" );
    {
	const VnusInt upperbound6 = (*A).GetSize0();

	for( VnusInt _card_i=0L; _card_i<upperbound6; ++_card_i )
	{
	    if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(_card_i,(*A).GetSize0()) ) )
	    {
		VnusEventArrayBoundViolated();
	    }
	    (*A)[_card_i] = 66L;
	}
    }
    _Print1dShape( _f, A );
    _Print1dShape( _f, A );
    _Print1dShape( _f, A );
    v__writeString( _f, "A[i] := 0;\n" );
    {
	const VnusInt upperbound7 = (*A).GetSize0();

	for( VnusInt i=0L; i<upperbound7; ++i )
	{
	    if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(i,(*A).GetSize0()) ) )
	    {
		VnusEventArrayBoundViolated();
	    }
	    (*A)[i] = 0L;
	}
    }
    v__writeString( _f, "A: " );
    _Print1dShape( _f, A );
    v__writeString( _f, "B[i] := i;\n" );
    {
	const VnusInt upperbound8 = (*B).GetSize0();

	for( VnusInt i=0L; i<upperbound8; ++i )
	{
	    if( VNUS_UNLIKELY( VnusIsUpperBoundViolated(i,(*B).GetSize0()) ) )
	    {
		VnusEventArrayBoundViolated();
	    }
	    (*B)[i] = i;
	}
    }
    v__writeString( _f, "B: " );
    _Print1dShape( _f, B );
    v__writeString( _f, "A[i] := B[i];\n" );
    {
	const VnusInt upperbound9 = (*A).GetSize0();

	for( VnusInt i=0L; i<upperbound9; ++i )
	{
	    if( VNUS_UNLIKELY( (VnusIsUpperBoundViolated(i,(*A).GetSize0())||VnusIsUpperBoundViolated(i,(*B).GetSize0())) ) )
	    {
		VnusEventArrayBoundViolated();
	    }
	    (*A)[i] = (*B)[i];
	}
    }
    v__writeString( _f, "A: " );
    _Print1dShape( _f, A );
    _testscopes( _f, 12L );
    testcasts();
    rec.field0 = 12L;
    rec.field1 = (VnusFloat) rec.field0;
    rec.field2 = 'x';
    rec = record1( 1L, 1, 'z' );
    prec = &rec;
    prec2 = (record2 *) prec;
    // Scope: scope0
    record1 rectmp0 = rec;

    rec2 = record2( rectmp0.field0, rectmp0.field1 );
    prec->field0 = prec2->field0;
    {
	// Scope: okblock
	VnusBoolean ok;

	ok = (prec==(record1 *) NULL);
	if( ok )
	{
	    v__writeString( _f, "Strange, pointer is null" );
	    v__writeString( _f, "\n" );
	}
	else
	{
	    v__writeString( _f, "Good, pointer is not null" );
	    v__writeString( _f, "\n" );
	}
    }
    prec2 = new record2;
    if( (prec2==(record2 *) NULL) )
    {
	v__writeString( _f, "Strange, pointer is null" );
	v__writeString( _f, "\n" );
    }
    delete prec2;
    /* garbagecollect(); */
    switch( 3L ){
	case 4:
	    v__writeString( _f, "four" );
	    v__writeString( _f, "\n" );

	case 3:
	    v__writeString( _f, "three" );
	    v__writeString( _f, "\n" );

	case 2:
	    ;

	case 1:
	    v__writeString( _f, "one" );
	    v__writeString( _f, "\n" );
	    goto next;

	default:
	    ;

	case 0:
	    v__writeString( _f, "zero" );
	    v__writeString( _f, "\n" );

    }
next:
    v__writeString( _f, "yes\n" );
    v__writeString( _f, "Yes" );
    v__writeString( _f, "\n" );
    v__writeString( _f, "left shifts: int:" );
    v__writeInt( _f, 4L );
    v__writeString( _f, " byte:" );
    v__writeByte( _f, (((VnusByte) 1)<<2L) );
    v__writeString( _f, " short:" );
    v__writeShort( _f, (((VnusShort) 1)<<2L) );
    v__writeString( _f, " long:" );
    v__writeLong( _f, (1LL<<2L) );
    v__writeString( _f, "\n" );
    v__writeString( _f, "right shifts: int:" );
    v__writeInt( _f, (55L>>2L) );
    v__writeString( _f, " byte:" );
    v__writeByte( _f, (((VnusByte) 55)>>2L) );
    v__writeString( _f, " short:" );
    v__writeShort( _f, (((VnusShort) 55)>>2L) );
    v__writeString( _f, " long:" );
    v__writeLong( _f, (55LL>>2L) );
    v__writeString( _f, "\n" );
    v__writeString( _f, "unsigned right shifts: int:" );
    v__writeInt( _f, ((VnusInt) (((UnsignedVnusInt)55L)>>2L)) );
    v__writeString( _f, " byte:" );
    v__writeByte( _f, ((VnusByte) (((UnsignedVnusByte)((VnusByte) 55))>>2L)) );
    v__writeString( _f, " short:" );
    v__writeShort( _f, ((VnusShort) (((UnsignedVnusShort)((VnusShort) 55))>>2L)) );
    v__writeString( _f, " long:" );
    v__writeLong( _f, ((VnusLong) (((UnsignedVnusLong)55LL)>>2L)) );
    v__writeString( _f, "\n" );
    v__writeString( _f, "shifts of negative numbers: left:" );
    v__writeInt( _f, (-44L) );
    v__writeString( _f, " right:" );
    v__writeInt( _f, ((-11L)>>2L) );
    v__writeString( _f, " unsigned right:" );
    v__writeInt( _f, ((VnusInt) (((UnsignedVnusInt)(-11L))>>2L)) );
    v__writeString( _f, "\n" );
    v__writeString( _f, "bitwise operations: and:" );
    v__writeInt( _f, 16L );
    v__writeString( _f, " or:" );
    v__writeInt( _f, 19L );
    v__writeString( _f, " xor:" );
    v__writeInt( _f, 3L );
    v__writeString( _f, "\n" );
    v__writeString( _f, "boolean operations: and:" );
    v__writeBoolean( _f, (true&false) );
    v__writeString( _f, " or:" );
    v__writeBoolean( _f, (true|false) );
    v__writeString( _f, " xor:" );
    v__writeBoolean( _f, (true^false) );
    v__writeString( _f, "\n" );
    v__writeString( _f, "shortcircuit boolean operations: and:" );
    v__writeBoolean( _f, false );
    v__writeString( _f, " or:" );
    v__writeBoolean( _f, true );
    v__writeString( _f, "\n" );
    v__writeString( _f, "bitwise not:" );
    v__writeInt( _f, (-13L) );
    v__writeString( _f, " " );
    v__writeInt( _f, 143L );
    v__writeString( _f, "\n" );
    v__writeString( _f, "boolean not:" );
    v__writeBoolean( _f, true );
    v__writeString( _f, " " );
    v__writeBoolean( _f, false );
    v__writeString( _f, "\n" );
    // Scope: scope1
    VnusInt where0;

    {
	// Scope: wherexpr
	VnusInt wherey = 3L;

	where0 = 9L;
    }
    v__writeString( _f, "wherey^2:" );
    v__writeInt( _f, where0 );
    v__writeString( _f, "\n" );
    _pp = &_Empty;
    _pp = &_Empty;
    // Scope: scope2
    void (*where1)();

    where1 = _pp;
    where1 = &_Empty;
    (*_pp)();
    {
	const VnusInt lowerbound0 = _i;

	for( VnusInt _testsec_i=0L, _testsec_j=12L, _testsec_k=lowerbound0; _testsec_i<12L; ++_testsec_i, _testsec_j+=(-1L), _testsec_k+=2L )
	{
	    v__writeString( _f, "i=" );
	    v__writeInt( _f, _testsec_i );
	    v__writeString( _f, " j=" );
	    v__writeInt( _f, _testsec_j );
	    v__writeString( _f, " k=" );
	    v__writeInt( _f, _testsec_k );
	    v__writeString( _f, "\n" );
	}
    }
    v__writeInt( _f, 1L );
    v__writeString( _f, "\n" );
    v__writeInt( _f, 36L );
    v__writeString( _f, "\n" );
    v__writeInt( _f, (((((((1L-2L)-3L)-4L)-5L)-6L)-7L)-8L) );
    v__writeString( _f, "\n" );
    v__close( _f );
    _f = 0L;
    exit( 0 );
    return 0;
}