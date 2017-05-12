/* File: service.h
 *
 * Header file for service.c
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "vnustypes.h"

// GCC 3.0 and later allow annotation with branch probabilities.
// Actually, GCC 2.96 also supports this, but we're not going to bother
// detecting that version.
//#if (defined( __GNUC__ ) && __GNUC__>2) || defined( __INTEL_COMPILER )
//#define VNUS_LIKELY(expression) (__builtin_expect((expression), 0))
//#define VNUS_UNLIKELY(expression) (__builtin_expect((expression), 1))
//#else
#define VNUS_LIKELY(expression) (expression)
#define VNUS_UNLIKELY(expression) (expression)
//#endif

extern void euclid(
    const VnusInt alpha, const VnusInt beta,
    VnusInt *x0, VnusInt *y0,
    VnusInt *g,
    VnusInt *dx, VnusInt *dy
);
//extern VnusInt floordiv( const VnusInt a, const VnusInt b );
//extern VnusInt ceildiv( const VnusInt a, const VnusInt b );
//extern void iterations(
//    const VnusInt l, const VnusInt u, const VnusInt s,
//    VnusInt *n, VnusInt *shift
//    );


/* NOTE NOTE NOTE NOTE NOTE
 * The code below is system-dependent. For the moment we assume
 * division rounds towards zero, but this may be different for other
 * machines.
 * NOTE NOTE NOTE NOTE NOTE
 */

/* Division with rounding down. */
inline VnusInt floordiv( VnusInt a, VnusInt b )
{
    if( b<0 ){
	a = -a;
	b = -b;
    }
    if( a>0 ){
	return a/b;
    }
    return (a-(b-1))/b;
}

/* Division with rounding up. */
inline VnusInt ceildiv( VnusInt a, VnusInt b )
{
    if( b<0 ){
	a = -a;
	b = -b;
    }
    if( a>0 ){
	return (a+(b-1))/b;
    }
    return a/b;
}

/* Range calculation. Given a lower bound 'l', upper bound 'u' and stride 's',
 * we assume an 'i' such that l <= i*s < u, (i in Z).
 * We calculate the lowest valid value of 'i', and assign it to
 * '*shift'. We also calculate the the number of valid values for 'i',
 * and assign it to '*n'.
 */
inline void iterations(
    const VnusInt l, const VnusInt u, const VnusInt s,
    VnusInt *n, VnusInt *shift
)
{
    *shift = ceildiv( l, s );
    *n = (VnusInt) ceildiv( u, s )-*shift;
}

inline VnusDouble VnusLongBitsToDouble( VnusLong n )
{
    VnusDouble d;
    VnusLong *p = reinterpret_cast<VnusLong *>( &d );

    *p = n;
    return d;
}

inline VnusFloat VnusIntBitsToFloat( VnusInt n )
{
    VnusFloat f;
    VnusInt *p = reinterpret_cast<VnusInt *>( &f );

    *p = n;
    return f;
}

#endif
