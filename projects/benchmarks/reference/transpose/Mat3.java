public class Mat3 {
    final static int M = 13*13;
    final static int iters = 40;

    public static void main( String args[] ){
	int A[][][] = new int[M][M][M];
	int B[][][] = new int[M][M][M];

	System.out.println( "Doing "+ M*M*M*iters+ " iterations" );
	for( int iter=0; iter<iters; iter++ ){
	    for( int i=0; i<M; i++ ){
		for( int j=0; j<M; j++ ){
		    for( int k=0; k<M; k++ ){
			A[i][j][k] = B[k][j][i];
		    }
		}
	    }
	}
    }
}
