public class Mat1 {
    final static int M = 1<<11;
    final static int N = M*M;
    final static int iters = 100;

    public static void main( String args[] ){
	int A[] = new int[N];

	System.out.println( "Doing " + N*iters  + " iterations" );
	for( int iter=0; iter<iters; iter++ ){
	    for( int i=0; i<N; i++ ){
		A[i] = iter;
	    }
	}
    }
}
