/* File: vnusrtl.h
 *
 * Include file for the run-time library for Vnus.
 */

#ifndef __VNUSRTL_H__
#define __VNUSRTL_H__

#include "vnusservice.h"
#include "shape.h"
#include "vnustypes.h"
#include "taskpar.h"

#define UPPERBOUND_HACK /**/

/* From bclus.cc */
inline void vnus_bclus1(
 const VnusInt m,
 const VnusInt np,
 const VnusInt a,
 const VnusInt b,
 VnusInt *n_u1,
 VnusInt *i0,
 VnusInt *j1_low,
 VnusInt *di,
 const VnusInt p
)
{
    VnusInt offset1;
    VnusInt r0;
    VnusInt g;
    VnusInt dr;

    euclid( a, np*m, i0, &r0, &g, di, &dr );
    offset1  = b - m*p;
    iterations( -offset1, m-offset1, g, n_u1, j1_low );
#ifdef UPPERBOUND_HACK
	/* FK: hack */
    if (*n_u1 < 0)
	*n_u1 = 0;
#endif
}

inline void vnus_bclus2(
 const VnusInt u1,
 const VnusInt i0,
 const VnusInt j1_low,
 const VnusInt ni,
 const VnusInt di,
 VnusInt *n_u0,
 VnusInt *offset0,
 const VnusInt p
)
{
    VnusInt t;
    VnusInt j0_low;

    (void) p;
    t = i0*u1+i0*j1_low;
    iterations( -t, ni-t, di, n_u0, &j0_low );
    *offset0 = t+j0_low*di;
#ifdef UPPERBOUND_HACK
	/* FK: hack */
    if (*n_u0 < 0)
	*n_u0 = 0;
#endif
}


/* From blus.cc */
inline void vnus_blus(
 const VnusInt m,
 const VnusInt /*np*/,
 const VnusInt a,
 const VnusInt b,
 const VnusInt ni,
 VnusInt *n_j0,
 VnusInt *j0_low,
 const VnusInt p
)
{
    VnusInt start;
    VnusInt end;
    const VnusInt stopelm = a*ni+b;
    const VnusInt startelm = b;

    start = m*p;
    end = m+start;
    if( a>0 ){
	if( start<startelm ){
	    start = startelm;
	}
	if( end>stopelm ){
	    end = stopelm;
	}
	iterations( start-b, end-b, a, n_j0, j0_low );
    }
    else {
	if( start<stopelm ){
	    start = stopelm;
	}
	if( end>startelm ){
	    end = startelm;
	}
	iterations( end-b, start-b, a, n_j0, j0_low );
    }
#ifdef UPPERBOUND_HACK
	/* FK: hack */
    if (*n_j0 < 0)
	*n_j0 = 0;
#endif
}

/* From clus.cc */
inline void vnus_clus(
 const VnusInt np,
 const VnusInt a,
 const VnusInt b,
 const VnusInt ni,
 VnusInt *di,
 VnusInt *n_u0,
 VnusInt *offset0,
 const VnusInt p
)
{
    VnusInt r0;
    VnusInt g;
    VnusInt dr;
    VnusInt j0_low;
    VnusInt i0;

    euclid( a, np, &i0, &r0, &g, di, &dr );
    if ( (p-b) % g != 0)
    {
	*di = 1;
	*n_u0 = 0;
	*offset0 = 0;
	return;
    }
    VnusInt j1_low = ceildiv( p-b, g );
    VnusInt t = i0*j1_low;
    iterations( -t, ni-t, *di, n_u0, &j0_low );
    *offset0 = t+j0_low*(*di);
#ifdef UPPERBOUND_HACK
	/* FK: hack */
    if (*n_u0 < 0)
	*n_u0 = 0;
#endif
}

/* Allocator functions */
inline VnusByte* vnus_new(VnusInt nelm, VnusInt elmsz)
     //{ return (VnusByte*)calloc(nelm, elmsz); }
{ return (VnusByte*)malloc(nelm * elmsz); }

inline void vnus_delete(VnusByte* ptr)
{ free(ptr); }
      
/* From thread.cc, pvm.cc and mpi.cc */
extern VnusInt thisProcessor();
extern VnusBoolean isThisProcessor(const VnusInt procNo);

extern void vnus_exec_forkall(
 void (*body)( int p, int, char** ),
 const int procno,
 const int argc,
 char** argv
);

extern void vnus_barrier( void );

extern void vnus_wait_pending();


extern void vnus_send(
 const int dest,
 const int size,
 const void *buf
);

extern void vnus_receive(
 const int src,
 const int size,
 void *buf
);

extern void vnus_broadcast(
 const int size,
 const void *buf
);

extern void vnus_send_real(
 const int dest,
 const int size,
 const void *buf
);

extern void vnus_asend(
 const int dest,
 const int size,
 const void *buf
);

extern void vnus_areceive(
 const int src,
 const int size,
 void *buf
);

extern void vnus_abroadcast(
 const int size,
 const void *buf
);

extern void vnus_asend_real(
 const int dest,
 const int size,
 const void *buf
);

#endif
