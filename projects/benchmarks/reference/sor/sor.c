#include <stdlib.h>

#define iters 30
#define N 2000

typedef struct str_wrapper {
    int n;
} wrapper;

int main( int argc, char *argv[] )
{
    wrapper w;
    wrapper *p = &w;
    double *A = (double *) calloc( N*N, sizeof(double) );
    const double f = 0.75;
    int i;
    int j;
    int n;

    w.n = N;
    A[N/2+p->n*N/2] = 100;
    for( n=0; n<iters; n++ ){
	for( j=1; j<N-1; j++ ){
	    int w = N*j;
	    int wm1 = N*(j-1);
	    int wp1 = N*(j+1);
	    //int w = p->n*j;
	    //int wm1 = p->n*(j-1);
	    //int wp1 = p->n*(j+1);
	    for( i=1; i<N-1; i++ ){
		A[i+w] = f*A[i+w] + (1-f)*(A[i+wm1]+A[i+wp1]+A[i+1+w]+A[i-1+w])/4;
	    }
	}
    }
    exit(0);
    return 0;
}
