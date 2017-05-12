// File: okswitchdefass.java
//
// Test wether the analysis is sophisticated enough to know that a variable
// is definitely assigned after it has been assigned in all switch cases,
// including default.

public class okswitchdefass {
    static void main( String args[] ){
	int n;

	switch( args.length ){
	    case 0: n = 1;	break;
	    default: n = 2;	break;
	}
	System.out.println( n );
    }
}
