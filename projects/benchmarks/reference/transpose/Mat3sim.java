public class Mat3sim {
    final static int M = 1<<7;
    final static int iters = 200;

    public static void main( String args[] ){
	int A[] = new int[M*M*M];

	__println( 1, "Doing ", M*M*M*iters, " iterations" );
	for( int iter=0; iter<iters; iter++ ){
	    for( int i=0; i<M; i++ ){
		int wi = i*M*M;
		for( int j=0; j<M; j++ ){
		    int wj = j*M;
		    for( int k=0; k<M; k++ ){
			A[wi+wj+k] = iter;
		    }
		}
	    }
	}
    }
}
