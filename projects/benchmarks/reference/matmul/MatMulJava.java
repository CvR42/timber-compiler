// File: MatMulJava.java
//
// Reference benchmark program
//
// After the matrix multiplication benchmark shown by the Ninja
// compiler people.

public class MatMulJava {
    static void matmul( double [][] A, double [][] B, double [][] C,
		      int m, int n, int p ){
	int i,j,k;

	for( i=0; i<m; i++ )
	    for( j=0; j<p; j++ )
		for( k=0; k<n; k++ )
		    C[i][j] += A[i][k]*B[k][j];
    }
    public static void main( String args[] ){
	if( args.length != 1 ){
	    System.err.println( "Exactly one command line argument required" );
	    System.exit( 1 );
	}
	String s = args[0];
	if( s.equals( "small" ) ){
	    int sz = 64;
	    double [][] A = new double[sz][sz];
	    double [][] B = new double[sz][sz];
	    double [][] C = new double[sz][sz];

	    matmul( A, B, C, sz, sz, sz );
	}
	else if( s.equals( "large" ) ){
	    int sz = 500;
	    double [][] A = new double[sz][sz];
	    double [][] B = new double[sz][sz];
	    double [][] C = new double[sz][sz];

	    matmul( A, B, C, sz, sz, sz );
	}
	else {
	    System.err.println( "Argument should be `small' or `large', not `" + s + "'" );
	    System.exit( 1 );
	}
    }
}
