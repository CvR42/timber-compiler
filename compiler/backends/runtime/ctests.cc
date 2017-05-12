/* File: test.cc
 *
 * Test the runtime library routines.
 */

//#define TRACE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "shape.h"

#define NPROC 5
#define ARRAYSIZE 100
#define OWN(i) ((i) % NPROC)

#ifdef HAVE_PTHREADS
static int the_array[ARRAYSIZE];
static int other_array[ARRAYSIZE];
#endif

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)>(b))?(a):(b))

VnusInt numberOfProcessors = NPROC;
VnusInt processorArrayInit[] = {NPROC};

/* Test ShapeDn accesses. */
static void test_Shapes( void )
{
    ShapeD0<VnusInt> *A0 = ShapeD0<VnusInt>::create_shape( NULL );
    ShapeD1<VnusInt> *A1 = ShapeD1<VnusInt>::create_shape( NULL, 20 );
    ShapeD2<VnusInt> *A2 = ShapeD2<VnusInt>::create_shape( NULL, 5, 12 );
    ShapeD3<VnusInt> *A3 = ShapeD3<VnusInt>::create_shape( NULL, 7, 5, 15 );
    VnusInt i, j, k;
    VnusInt val;

    printf( "Testing 0-dimentional access\n" );
    (*A0)() = 0;
    if( (*A0)() != 0 ){
        printf( "A0[] not zero\n" );
	return;
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        (*A1)(i) = 0;
    }
    (*A0)() = (*A0)() + 1;
    val = (*A0)();
    if( val<1 ){
	printf( "A0[] = %ld not incremented\n", (long) val );
	return;
    }
    if( val>1 ){
	printf( "A0[] = %ld incremented more than once\n", (long) val );
	return;
    }
    A0->AccessChecked() = A0->AccessChecked() + 1;
    val = (*A0)();
    if( val<2 ){
	printf( "A0[] = %ld not incremented\n", (long) val );
	return;
    }
    if( val>2 ){
	printf( "A0[] = %ld incremented more than once\n", (long) val );
	return;
    }
    A0->AccessNotChecked() = A0->AccessNotChecked() + 1;
    val = (*A0)();
    if( val<3 ){
	printf( "A0[] = %ld not incremented\n", (long) val );
	return;
    }
    if( val>3 ){
	printf( "A0[] = %ld incremented more than once\n", (long) val );
	return;
    }
    printf( "Testing 1-dimentional access\n" );
    for( i=0; i<A1->GetSize(0); i++ ){
        if( (*A1)(i) != 0 ){
	    printf( "A1[%ld] not zero\n", (long) i );
	    return;
        }
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        (*A1)(i) = (*A1)(i) + 1;
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        val = (*A1)(i);
        if( val<1 ){
	    printf( "A1[%ld] = %ld not incremented\n", (long) i, (long) val );
	    return;
        }
        if( val>1 ){
	    printf( "A1[%ld] = %ld incremented more than once\n", (long) i, (long) val );
	    return;
        }
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        A1->AccessChecked(i) = A1->AccessChecked(i) + 1;
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        val = (*A1)(i);
        if( val<2 ){
	    printf( "A1[%ld] = %ld not incremented\n", (long) i, (long) val );
	    return;
        }
        if( val>2 ){
	    printf( "A1[%ld] = %ld incremented more than once\n", (long) i, (long) val );
	    return;
        }
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        A1->AccessNotChecked(i) = A1->AccessNotChecked(i) + 1;
    }
    for( i=0; i<A1->GetSize(0); i++ ){
        val = (*A1)(i);
        if( val<3 ){
	    printf( "A1[%ld] = %ld not incremented\n", (long) i, (long) val );
	    return;
        }
        if( val>3 ){
	    printf( "A1[%ld] = %ld incremented more than once\n", (long) i, (long) val );
	    return;
        }
    }
    printf( "Testing 2-dimentional access\n" );
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    (*A2)(i,j) = 0;
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    if( (*A2)(i,j) != 0 ){
		printf( "A2[%ld,%ld] not zero\n", (long) i, (long) j );
		return;
	    }
        }
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    (*A2)(i,j) = (*A2)(i,j) + 1;
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    val = (*A2)(i,j);
	    if( val<1 ){
		printf( "A2[%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) val );
		return;
	    }
	    if( val>1 ){
		printf( "A2[%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) val );
		return;
	    }
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    A2->AccessChecked(i,j) = A2->AccessChecked(i,j) + 1;
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    val = (*A2)(i,j);
	    if( val<2 ){
		printf( "A2[%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) val );
		return;
	    }
	    if( val>2 ){
		printf( "A2[%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) val );
		return;
	    }
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    A2->AccessNotChecked(i,j) = A2->AccessNotChecked(i,j) + 1;
	}
    }
    for( i=0; i<A2->GetSize(0); i++ ){
	for( j=0; j<A2->GetSize(1); j++ ){
	    val = (*A2)(i,j);
	    if( val<3 ){
		printf( "A2[%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) val );
		return;
	    }
	    if( val>3 ){
		printf( "A2[%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) val );
		return;
	    }
	}
    }
    printf( "Testing 3-dimentional access\n" );
    for( i=0; i<A3->GetSize(0); i++ ){
        for( j=0; j<A3->GetSize(1); j++ ){
            for( k=0; k<A3->GetSize(2); k++ ){
                (*A3)(i,j,k) = 0;
            }
        }
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		if( (*A3)(i,j,k) != 0 ){
		    printf( "A3[%ld,%ld,%ld] not zero\n", (long) i, (long) j, (long) k );
		    return;
		}
	    }
        }
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		(*A3)(i,j,k) = (*A3)(i,j,k) + 1;
	    }
	}
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		val = (*A3)(i,j,k);
		if( val<1 ){
		    printf( "A3[%ld,%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
		if( val>1 ){
		    printf( "A3[%ld,%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
	    }
	}
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		A3->AccessChecked(i,j,k) = A3->AccessChecked(i,j,k) + 1;
	    }
	}
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		val = (*A3)(i,j,k);
		if( val<2 ){
		    printf( "A3[%ld,%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
		if( val>2 ){
		    printf( "A3[%ld,%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
	    }
	}
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		A3->AccessNotChecked(i,j,k) = A3->AccessNotChecked(i,j,k) + 1;
	    }
	}
    }
    for( i=0; i<A3->GetSize(0); i++ ){
	for( j=0; j<A3->GetSize(1); j++ ){
	    for( k=0; k<A3->GetSize(2); k++ ){
		val = (*A3)(i,j,k);
		if( val<3 ){
		    printf( "A3[%ld,%ld,%ld] = %ld not incremented\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
		if( val>3 ){
		    printf( "A3[%ld,%ld,%ld] = %ld incremented more than once\n", (long) i, (long) j, (long) k, (long) val );
		    return;
		}
	    }
	}
    }
}

// Test constant Shapes.
static void test_const_Shapes()
{
    VnusInt arr1[] = { 1, 2, 3 };
    ShapeD1<VnusInt> *IA1 = ShapeD1<VnusInt>::create_init_shape( NULL, arr1, 3 );

    for( int i=0; i<IA1->GetSize(0); i++ ){
	printf( "IA1[%d]=%d\n", i, (*IA1)(i) );
    }
}

/* Test ceildiv() and floordiv(). */
static void test_divs( void )
{
    VnusInt res;

    printf( "Testing ceildiv() and floordiv()...\n" );
    res = floordiv( 4, 2 );
    if( res != 2 ){
	printf( "Strange, floordiv( 4, 2 ) is %ld\n", (long) res );
    }
    res = floordiv( 5, 2 );
    if( res != 2 ){
	printf( "Strange, floordiv( 5, 2 ) is %ld\n", (long) res );
    }
    res = floordiv( 5, 7 );
    if( res != 0 ){
	printf( "Strange, floordiv( 5, 7 ) is %ld\n", (long) res );
    }
    res = floordiv( -4, 2 );
    if( res != -2 ){
	printf( "Strange, floordiv( -4, 2 ) is %ld\n", (long) res );
    }
    res = floordiv( -5, 2 );
    if( res != -3 ){
	printf( "Strange, floordiv( -5, 2 ) is %ld\n", (long) res );
    }
    res = floordiv( -5, 7 );
    if( res != -1 ){
	printf( "Strange, floordiv( -5, 7 ) is %ld\n", (long) res );
    }
    res = floordiv( 4, -2 );
    if( res != -2 ){
	printf( "Strange, floordiv( 4, -2 ) is %ld\n", (long) res );
    }
    res = floordiv( 5, -2 );
    if( res != -3 ){
	printf( "Strange, floordiv( 5, -2 ) is %ld\n", (long) res );
    }
    res = floordiv( 5, -7 );
    if( res != -1 ){
	printf( "Strange, floordiv( 5, -7 ) is %ld\n", (long) res );
    }
    res = floordiv( -4, -2 );
    if( res != 2 ){
	printf( "Strange, floordiv( -4, -2 ) is %ld\n", (long) res );
    }
    res = floordiv( -5, -2 );
    if( res != 2 ){
	printf( "Strange, floordiv( -5, -2 ) is %ld\n", (long) res );
    }
    res = floordiv( -5, -7 );
    if( res != 0 ){
	printf( "Strange, floordiv( -5, 7 ) is %ld\n", (long) res );
    }
    /* ceildiv() */
    res = ceildiv( 4, 2 );
    if( res != 2 ){
	printf( "Strange, ceildiv( 4, 2 ) is %ld\n", (long) res );
    }
    res = ceildiv( 5, 2 );
    if( res != 3 ){
	printf( "Strange, ceildiv( 5, 2 ) is %ld\n", (long) res );
    }
    res = ceildiv( 5, 7 );
    if( res != 1 ){
	printf( "Strange, ceildiv( 5, 7 ) is %ld\n", (long) res );
    }
    res = ceildiv( -4, 2 );
    if( res != -2 ){
	printf( "Strange, ceildiv( -4, 2 ) is %ld\n", (long) res );
    }
    res = ceildiv( -5, 2 );
    if( res != -2 ){
	printf( "Strange, ceildiv( -5, 2 ) is %ld\n", (long) res );
    }
    res = ceildiv( -5, 7 );
    if( res != 0 ){
	printf( "Strange, ceildiv( -5, 7 ) is %ld\n", (long) res );
    }
    res = ceildiv( 4, -2 );
    if( res != -2 ){
	printf( "Strange, ceildiv( 4, -2 ) is %ld\n", (long) res );
    }
    res = ceildiv( 5, -2 );
    if( res != -2 ){
	printf( "Strange, ceildiv( 5, -2 ) is %ld\n", (long) res );
    }
    res = ceildiv( 5, -7 );
    if( res != 0 ){
	printf( "Strange, ceildiv( 5, -7 ) is %ld\n", (long) res );
    }
    res = ceildiv( -4, -2 );
    if( res != 2 ){
	printf( "Strange, ceildiv( -4, -2 ) is %ld\n", (long) res );
    }
    res = ceildiv( -5, -2 );
    if( res != 3 ){
	printf( "Strange, ceildiv( -5, -2 ) is %ld\n", (long) res );
    }
    res = ceildiv( -5, -7 );
    if( res != 1 ){
	printf( "Strange, ceildiv( -5, 7 ) is %ld\n", (long) res );
    }
    printf( "Testing ceildiv() and floordiv() completed\n" );
}

/* Test the euclid routine with the given parameters. */
static void test_one_euclid( const VnusInt a, const VnusInt b )
{
    VnusInt x0;
    VnusInt y0;
    VnusInt g;
    VnusInt dx;
    VnusInt dy;

    printf( "testing euclid( %ld, %ld )\n", (long) a, (long) b );
    euclid( a, b, &x0, &y0, &g, &dx, &dy );
    if( g != x0*a-y0*b ){
	printf( "Bad g found:\n" );
	printf(
	    "a=%ld, b=%ld, x0=%ld, y0=%ld, g=%ld, dx=%ld, dy=%ld\n",
	    (long) a,
	    (long) b,
	    (long) x0,
	    (long) y0,
	    (long) g,
	    (long) dx,
	    (long) dy
	);
    }
    if( dx*g != b ){
	printf( "Bad dx found:\n" );
	printf(
	    "a=%ld, b=%ld, x0=%ld, y0=%ld, g=%ld, dx=%ld, dy=%ld\n",
	    (long) a,
	    (long) b,
	    (long) x0,
	    (long) y0,
	    (long) g,
	    (long) dx,
	    (long) dy
	);
    }
    if( dy*g != a ){
	printf( "Bad dy found:\n" );
	printf(
	    "a=%ld, b=%ld, x0=%ld, y0=%ld, g=%ld, dx=%ld, dy=%ld\n",
	    (long) a,
	    (long) b,
	    (long) x0,
	    (long) y0,
	    (long) g,
	    (long) dx,
	    (long) dy
	);
    }
}

/* Test the euclid routine. */
static void test_euclid( void )
{
    printf( "Testing euclid()...\n" );
    test_one_euclid( 3, 14 );
    test_one_euclid( -3, 14 );
    test_one_euclid( 3, -14 );
    test_one_euclid( -3, -14 );
    test_one_euclid( 4, 2 );
    test_one_euclid( -4, 2 );
    test_one_euclid( -4, -2 );
    test_one_euclid( 4, -2 );
    test_one_euclid( 200, 7 );
    test_one_euclid( -200, 7 );
    test_one_euclid( -200, 1 );
    test_one_euclid( 200, 1 );
    test_one_euclid( 0, 1 );
    printf( "Testing euclid() completed.\n" );
}

/* Test the iteration routine for given lower bound, upper bound and stride. */
static void test_one_iteration( const VnusInt l, const VnusInt u, const VnusInt s )
{
    VnusInt predict_n;
    VnusInt predict_low;
    VnusInt count_n;
    VnusInt i;
    VnusInt sgn;

    iterations( l, u, s, &predict_n,&predict_low );
    predict_low *= s;
    sgn = (s<0?-1:1);
    if( s>0 ){
	if( predict_low<l || predict_low-s>=l ){
	    printf( "l: %ld, u: %ld, s: %ld\n", (long) l, (long) u, (long) s );
	    printf( "bad lower bound: predict_low=%ld\n", (long) predict_low );
	}
    }
    else {
	if( predict_low>l || predict_low-s<=l ){
	    printf( "l: %ld, u: %ld, s: %ld\n", (long) l, (long) u, (long) s );
	    printf( "bad lower bound: predict_low=%ld\n", (long) predict_low );
	}
    }
    count_n = 0;
    for( i=predict_low; sgn*i<sgn*u; i += s ){
	count_n++;
    }
    if( predict_n != count_n ){
	printf( "l: %ld, u: %ld, s: %ld\n", (long) l, (long) u, (long) s );
	printf(
	    "Predicted number of iterations: %ld, actual: %ld\n", 
	    (long) predict_n,
	    (long) count_n
	);
    }
}

/* Test the iterations routine. */
static void test_iterations( void )
{
    printf( "Testing iterations()...\n" );
    test_one_iteration( 0, 3, 1 );
    test_one_iteration( 0, 3, 2 );
    test_one_iteration( 0, 25, 2 );
    test_one_iteration( 0, 25, 7 );
    test_one_iteration( -12, 25, 7 );
    test_one_iteration( 12, 25, 7 );
    test_one_iteration( 0, 0, 7 );
    test_one_iteration( 0, -3, -1 );
    test_one_iteration( 3, 0, -1 );
    test_one_iteration( 3, 0, -2 );
    printf( "Testing iterations() completed.\n" );
}

/* Define some of the other distribution parameters. */
#define M 7
#define NI 30

#define NOT_VISITED 0
#define VISITED 1

static void test_one_bclus( const int a, const int b )
{
    VnusInt i;
    VnusInt p;
    VnusInt arrsz = max( NI*a+b, b )+20;
    ShapeD1<int> *arr = ShapeD1<int>::create_shape( NULL, arrsz );

    printf( "Testing bclus for a=%d, b=%d\n", a, b );
    assert( (NI*a+b)>0 );
    for( i=0; i<arrsz; i++ ){
	(*arr)(i) = NOT_VISITED;
    }
    for( p=0; p<numberOfProcessors; p++ ){
#ifdef NAIVE
	for( i=0; i<NI; i++ ){
	    if( arr.IsOwnerChecked( p, i ) ){
		if( arr(a*i+b) == VISITED ){
		    printf( "Element i=%ld already visited.\n", (long) i );
		}
		(*arr)(a*i+b) = VISITED;
	    }
	}
#else
	VnusInt i0;
	VnusInt u0;
	VnusInt u1;
	VnusInt n_u0;
	VnusInt n_u1;
	VnusInt j1_low;
	VnusInt offset0;
	VnusInt di;

	vnus_bclus1(
	    M, //arr.GetBlockSize(0),
	    numberOfProcessors,
	    a,
	    b,
	    &n_u1,
	    &i0,
	    &j1_low,
	    &di,
	    p
	);
	for( u1=0; u1<n_u1; u1++ ){
	    vnus_bclus2( u1, i0, j1_low, NI, di, &n_u0, &offset0, p );
	    for( u0 = 0; u0<n_u0; u0++ ){
		i = offset0+di*u0;
		if( (*arr)(a*i+b) == VISITED ){
		    printf( "Element i=%ld already visited.\n", (long) i );
		}
		(*arr)(a*i+b) = VISITED;
	    }
	}
#endif
    }
    for( i=0; i<NI; i++ ){
	if( (*arr)(a*i+b) != VISITED ){
	    printf( "Element i=%ld not visited.\n", (long) i );
	}
	(*arr)(a*i+b) = NOT_VISITED;
    }
    for( i=0; i<arrsz; i++ ){
	if( (*arr)(i) == VISITED ){
	    printf( "Element i=%ld illegally visited.\n", (long) i );
	}
    }
}

static void test_one_blus( const int a, const int b )
{
    VnusInt i;
    VnusInt p;
    VnusInt arrsz = max(a*NI+b,b+1)+1;
    ShapeD1<int> *arr = ShapeD1<int>::create_shape( NULL, arrsz );

    printf( "Testing blus for a=%d, b=%d\n", a, b );
    assert( (NI*a+b)>0 );
    for( i=0; i<arrsz; i++ ){
	(*arr)(i) = NOT_VISITED;
    }
    for( p=0; p<numberOfProcessors; p++ ){
#ifdef NAIVE
	for( i=0; i<NI; i++ ){
	    if( arr.IsOwnerChecked( p, i ) ){
		if( (*arr)(a*i+b) == VISITED ){
		    printf( "Element i=%ld already visited.\n", (long) i );
		}
		(*arr)(a*i+b) = VISITED;
	    }
	}
#else
	VnusInt n_u0;
	VnusInt j0_low;

	vnus_blus(
	    (arrsz+NPROC-1)/NPROC, //arr.GetBlockSize(0),
	    numberOfProcessors,
	    a,
	    b,
	    NI,
	    &n_u0,
	    &j0_low,
	    p
	);
	for( VnusInt u0 = 0; u0<n_u0; u0++ ){
	    i = u0+j0_low;
	    if( (*arr)(a*i+b) == VISITED ){
		printf( "Element arr[%ld] already visited.\n", (long) i );
	    }
	    (*arr)(a*i+b) = VISITED;
	}
#endif
    }
    for( i=0; i<NI; i++ ){
	if( (*arr)(a*i+b) != VISITED ){
	    printf( "Element i=%ld not visited.\n", (long) i );
	}
	(*arr)(a*i+b) = NOT_VISITED;
    }
    for( i=0; i<arrsz; i++ ){
	if( (*arr)(i) == VISITED ){
	    printf( "Element arr[%ld] illegally visited.\n", (long) i );
	}
    }
}

static void test_one_clus( const int a, const int b )
{
    VnusInt i;
    VnusInt p;
    VnusInt arrsz = max( NI*a+b, b )+20;
    ShapeD1<int> *arr = ShapeD1<int>::create_shape( NULL, arrsz );

    printf( "Testing clus for a=%d, b=%d\n", a, b );
    assert( (NI*a+b)>0 );
    for( i=0; i<arrsz; i++ ){
	(*arr)(i) = NOT_VISITED;
    }
    for( p=0; p<numberOfProcessors; p++ ){
#ifdef NAIVE
	for( i=0; i<NI; i++ ){
	    if( arr.IsOwnerChecked( p, i ) ){
		if( (*arr)(a*i+b) == VISITED ){
		    printf( "Element i=%ld already visited.\n", (long) i );
		}
		(*arr)(a*i+b) = VISITED;
	    }
	}
#else
	VnusInt u0;
	VnusInt n_u0;
	VnusInt offset0;
	VnusInt di;

	vnus_clus(
	    numberOfProcessors,
	    a,
	    b,
	    NI,
	    &di,
	    &n_u0,
	    &offset0,
	    p
	);
	for( u0 = 0; u0<n_u0; u0++ ){
	    i = offset0+di*u0;
	    if( (*arr)(a*i+b) == VISITED ){
		printf( "Element i=%ld already visited.\n", (long) i );
	    }
	    (*arr)(a*i+b) = VISITED;
	}
#endif
    }
    for( i=0; i<NI; i++ ){
	if( (*arr)(a*i+b) != VISITED ){
	    printf( "Element i=%ld not visited.\n", (long) i );
	}
	(*arr)(a*i+b) = NOT_VISITED;
    }
    for( i=0; i<arrsz; i++ ){
	if( (*arr)(i) == VISITED ){
	    printf( "Element i=%ld illegally visited.\n", (long) i );
	}
    }
}

/* Test the vnus_bclus routine. */
static void test_bclus( void )
{
    numberOfProcessors = NPROC;
    printf( "Testing vnus_bclus()...\n" );
    test_one_bclus( 3, 0 );
    test_one_bclus( 3, 1 );
    test_one_bclus( 1, 0 );
    test_one_bclus( 1, 1 );
    test_one_bclus( 1, 2 );
    test_one_bclus( 1, M+2 );
    test_one_bclus( 2, 1 );
    test_one_bclus( 4, 1 );
    test_one_bclus( M, 1 );
    test_one_bclus( M+1, 1 );
    test_one_bclus( 2*M, 1 );
    test_one_bclus( 2*M+2, 1 );
    test_one_bclus( -1, NI+1 );
    test_one_bclus( -2, 2*NI+1 );
    test_one_bclus( -2, 2*NI+5 );
    test_one_bclus( -4, 4*NI+8 );
    printf( "Testing vnus_bclus() completed.\n" );
}

/* Test the vnus_blus routine. */
static void test_blus( void )
{
    numberOfProcessors = NPROC;
    printf( "Testing vnus_blus()...\n" );
    test_one_blus( 3, 0 );
    test_one_blus( 3, 1 );
    test_one_blus( 1, 0 );
    test_one_blus( 1, 1 );
    test_one_blus( 1, 2 );
    test_one_blus( 1, M+2 );
    test_one_blus( 2, 1 );
    test_one_blus( 4, 1 );
    test_one_blus( M, 1 );
    test_one_blus( M+1, 1 );
    test_one_blus( 2*M, 1 );
    test_one_blus( 2*M+2, 1 );
    test_one_blus( -1, NI+1 );
    test_one_blus( -2, 2*NI+1 );
    test_one_blus( -2, 2*NI+5 );
    test_one_blus( -4, 4*NI+8 );
    printf( "Testing vnus_blus() completed.\n" );
}

/* Test the vnus_clus routine. */
static void test_clus( void )
{
    numberOfProcessors = NPROC;
    printf( "Testing vnus_clus()...\n" );
    test_one_clus( 3, 0 );
    test_one_clus( 3, 1 );
    test_one_clus( 1, 0 );
    test_one_clus( 1, 1 );
    test_one_clus( 1, 2 );
    test_one_clus( 1, M+2 );
    test_one_clus( 2, 1 );
    test_one_clus( 4, 1 );
    test_one_clus( M, 1 );
    test_one_clus( M+1, 1 );
    test_one_clus( 2*M, 1 );
    test_one_clus( 2*M+2, 1 );
    test_one_clus( -1, NI+1 );
    test_one_clus( -2, 2*NI+1 );
    test_one_clus( -2, 2*NI+5 );
    test_one_clus( -4, 4*NI+8 );
    test_one_clus( NPROC, 1 );
    printf( "Testing vnus_clus() completed.\n" );
}

#ifdef HAVE_PTHREADS
/* One computation thread: increment the elements you own, insert
 * a barrier, and then copy the elements up one slot, and insert
 * a zero at the lowest element. this should result in the same
 * array.
 */
static void a_thread( int p, int /*argc*/, char** /*argv*/ )
{
    int i;

    for( i=0; i<ARRAYSIZE; i++ ){
	if( OWN(i) == p ){
	    the_array[i]++;
	}
    }
    vnus_barrier();
    i = ARRAYSIZE;
    while( i>0 ){
	i--;
	if( OWN(i) == p ){
	    the_array[i] = the_array[i-1];
	}
	vnus_barrier();
    }
    if( OWN(0) == p ){
	the_array[0] = 0;
    }
    vnus_barrier();
}

/* Test the thread functions. */
static void test_threads( int argc, char** argv )
{
    int i;

    printf( "Starting the thread test.\n" );
    for( i=0; i<ARRAYSIZE; i++ ){
	the_array[i] = i;
    }
    vnus_exec_forkall( a_thread, NPROC, argc, argv );
    for( i=0; i<ARRAYSIZE; i++ ){
	if( the_array[i] != i ){
	    fprintf(
		stderr,
		"Array element %d should be %d, not %d.\n",
		i,
		i,
		the_array[i]
	    );
	}
    }
    printf( "Thread test completed.\n" );
}

/* One communication thread: send each element i that you own to the
 * processor that owns (ARRAYSIZE-1)-i.
 */
static void a_communication_thread( int p, int /*argc*/, char** /*argv*/ )
{
    int i;
    int desti;

    /* This barrier ensures that the other threads exist before this
     * thread tries to communicate with them.
     */
    vnus_barrier();
    /* This implements, in a roundabout way:
     * for( i=0; i<ARRAYSIZE; i++ ){
     *     desti = (ARRAYSIZE-1)-i;
     *     other_array[desti] = the_array[i];
     * }
     */
    for( i=0; i<ARRAYSIZE; i++ ){
	desti = (ARRAYSIZE-1)-i;
	if( OWN(i) == p ){
	    if( OWN( desti ) == p ){
		other_array[desti] = the_array[i];
#ifdef TRACE
		printf( "p=%d: locally copying element %d\n", p, i );
#endif
	    }
	    else {
#ifdef TRACE
		printf( "p=%d: sending element %d to %d\n", p, i, OWN(desti) );
#endif
		vnus_send(
		    OWN( desti ),
		    sizeof( the_array[i] ),
		    (void *) (the_array+i)
		);
	    }
	}
	if( OWN( desti ) == p && OWN( i ) != p ){
#ifdef TRACE
	    printf( "p=%d: receiving element %d from %d\n", p, i, OWN(i) );
#endif
	    vnus_receive(
		OWN( i ),
		sizeof( other_array[desti] ),
		(void *) (other_array+desti)
	    );
	}
	fflush( stdout );
    }
#ifdef TRACE
    printf( "p=%d: I've done my share\n", p );
#endif
    vnus_barrier();
    i = ARRAYSIZE;
    while( i>0 ){
	i--;
	if( OWN(i) == p ){
	    the_array[i] = the_array[i-1];
	}
	vnus_barrier();
    }
    if( OWN(0) == p ){
	the_array[0] = 0;
    }
    vnus_barrier();
}

/* Test the communication functions. */
static void test_communication( int argc, char** argv )
{
    int i;
    int goodi;

    printf( "Starting the communication test.\n" );
    for( i=0; i<ARRAYSIZE; i++ ){
	the_array[i] = i;
    }
    vnus_exec_forkall( a_communication_thread, NPROC, argc, argv );
    for( i=0; i<ARRAYSIZE; i++ ){
	goodi = (ARRAYSIZE-1)-i;
	if( other_array[i] != goodi ){
	    fprintf(
		stderr,
		"Array element %d should be %d, not %d.\n",
		i,
		goodi,
		other_array[i]
	    );
	}
    }
    printf( "Communication test completed.\n" );
}
#endif

int main( int argc, char *argv[] )
{
    (void) argc;
    (void) argv;

    test_Shapes();
    test_const_Shapes();
    test_divs();
    test_euclid();
    test_iterations();
    test_bclus();
    test_blus();
    test_clus();
#ifdef HAVE_PTHREADS
    test_threads( argc, argv );
    test_communication( argc, argv );
#endif
    exit( 0 );
    return 0;
}
