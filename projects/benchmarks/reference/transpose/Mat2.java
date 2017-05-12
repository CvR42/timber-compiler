public class Mat2 {
    final static int X = 13;
    final static int M = X*X*X;
    final static int iters = 40;

    public static void main( String args[] ){
	int A[][] = new int[M][M];
	int B[][] = new int[M][M];

	System.out.println( "Doing "+ M*M*iters+ " iterations" );
	for( int iter=0; iter<iters; iter++ ){
	    for( int i=0; i<M; i++ ){
		for( int j=0; j<M; j++ ){
		    A[i][j] = B[j][i];
		}
	    }
	}
    }
}
