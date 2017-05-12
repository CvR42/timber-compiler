// File: okswitchassign.java
//
// Tests definite assignment analysis of switch statements.

public class okswitchassign {
    static void main( String args[] ){
	int n;

	switch( args.length ){
	    case 0: n = 1;
	    default: n = 2;
	}
	System.out.println( n );
    }
}
