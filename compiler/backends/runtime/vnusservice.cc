/* File: service.cc
 *
 * Service routines that are used internally in the runtime library
 * routines. They are put in a seperate file to be able to test them,
 * but not make them visible for Vnus programs. They do not clash
 * with Vnus routines because they do not contain an '_', and
 * all Vnus symbols will contain at least one.
 */

#include <stdio.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "vnusservice.h"

/* Euclid's algorithm. */
void euclid(
 const VnusInt alpha, const VnusInt beta,
 VnusInt *x0, VnusInt *y0,
 VnusInt *g,
 VnusInt *dx, VnusInt *dy
)
{
    VnusInt q_j, q_j1;
    VnusInt r_j, r_j1, r_j2;
    VnusInt s_j, s_j1, s_j2;
    VnusInt t_j, t_j1, t_j2;

    r_j = beta; r_j1 = alpha;
    s_j = 0;  s_j1 = 1;
    t_j = -1; t_j1 = 0;

    while( r_j != 0 ){
        r_j2 = r_j1;
        r_j1 = r_j;
        s_j2 = s_j1;
        s_j1 = s_j;
        t_j2 = t_j1;
        t_j1 = t_j;
        q_j1 = q_j;
        q_j = r_j2/r_j1;
        r_j = r_j2 - q_j*r_j1;
        s_j = s_j2 - q_j*s_j1;
        t_j = t_j2 - q_j*t_j1;
#if 0
        r_j_test = s_j*alpha - t_j*beta;
        if( r_j_test != r_j ){
	    fprintf(
		stdout,
		"r_j:%ld, r_j_test:%ld\n",
		(long) r_j,
		(long) r_j_test
	    );
        }
        fprintf(
	    stdout,
	    "q_j:%ld, r_j:%ld, s_j:%ld, t_j:%ld\n",
	    (long) q_j,
	    (long) r_j,
	    (long) s_j,
	    (long) t_j
	);
#endif
    }
    if( r_j1<0 ){
	*g = - r_j1;
	*dx = - beta/r_j1;
	*dy = - alpha/r_j1;
	*x0 = - s_j1;
	*y0 = - t_j1;
    }
    else {
	*g = r_j1;
	*dx = beta/r_j1;
	*dy = alpha/r_j1;
	*x0 = s_j1;
	*y0 = t_j1;
    }
}
