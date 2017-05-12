// File: Enigma.java
//
// Implement an enigma machine with a given set of wheels and reflector, and 
// run strings through it.
public class Enigma {
    private Wheel A;
    private Wheel B;
    private Wheel C;
    private Reflector ref;
    private final static boolean DEBUG = false;

    // The wiring of the eight original Enigma wheels I - VIII
    private String wheels[] = {
	"EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	"AJDKSIRUXBLHWTMCQGZNPYFVOE",
	"BDFHJLCPRTXVZNYEIWGAKMUSQO",
	"ESOVPZJAYQUIRHXLNFTGKDCMWB",
	"VZBRGITYUPSDNHLXAWMJQOFECK",
	"JPGVOUMFYQBENHZRDKASXLICTW",
	"NZJHGRCXMYSWBOUFAIVLPEKQDT",
	"FKQHTLXOCBJSPDZRAMEWNIUYGV"
    };

    // The first turnover positions of the original Enigma wheels.
    private int wheel_turnover1[] = {
	Letter.Q, Letter.E, Letter.V, Letter.J,
	Letter.Z, Letter.Z, Letter.Z, Letter.Z
    };


    // The second turnover positions of the original Enigma wheels.
    private int wheel_turnover2[] = {
	-1, -1, -1, -1,
	-1, Letter.M, Letter.M, Letter.M
    };

    public Enigma( Wheel A, Wheel B, Wheel C, Reflector ref ){
        this.A = A;
	this.B = B;
	this.C = C;
	this.ref = ref;
    }

    public boolean rotate(){
	boolean carry = C.rotate();
	if( carry ){
	    carry = B.rotate();
	    if( carry ){
	        carry = A.rotate();
	    }
	}
	return carry;
    }

    // Run a single Letter through the machine.
    public byte encode( byte c ){
        byte lA = A.encodeLeft( c );
	byte lB = B.encodeLeft( lA );
	byte lC = C.encodeLeft( lB );
	byte r = ref.encode( lC );
	byte rC = C.encodeRight( r );
	byte rB = B.encodeRight( rC );
	byte rA = A.encodeRight( rB );
	if( DEBUG ){
	    byte rAr = A.encodeLeft( rA );
	    byte rBr = B.encodeLeft( rB );
	    byte rCr = C.encodeLeft( rC );
	    byte rr = ref.encode( rCr );
	    byte lCr = C.encodeRight( rr );
	    byte lBr = B.encodeRight( lCr );
	    byte lAr = A.encodeRight( lBr );
	    if( rAr != rB || rBr != rC || rCr != r || rr != lC || c != lAr ){
		System.out.println( "Cannot reverse encoding:" );
		System.out.println( "state: A=" + A.getState() + " B=" + B.getState() + " C=" + C.getState() );
		System.out.println( "  A   B   C   |   C   B   A" );
		System.out.println(
		    Letter.LetterToChar( c ) + " > " +
		    Letter.LetterToChar( lA ) + " > " +
		    Letter.LetterToChar( lB ) + " > " +
		    Letter.LetterToChar( lC ) + " > " +
		    Letter.LetterToChar( r ) + " > " +
		    Letter.LetterToChar( rC ) + " > " +
		    Letter.LetterToChar( rB ) + " > " +
		    Letter.LetterToChar( rA )
		);
		System.out.println(
		    Letter.LetterToChar( lAr ) + " < " +
		    Letter.LetterToChar( lBr ) + " < " +
		    Letter.LetterToChar( lCr ) + " < " +
		    Letter.LetterToChar( rr ) + " < " +
		    Letter.LetterToChar( rCr ) + " < " +
		    Letter.LetterToChar( rBr ) + " < " +
		    Letter.LetterToChar( rAr ) + " < " +
		    Letter.LetterToChar( rA )
		);
		System.exit( 1 );
	    }
	}
	return rA;
    }

    public char encode( char c ){
        byte code = Letter.CharToLetter( c );
	
	code = encode( code );
	return Letter.LetterToChar( code );
    }

    public char[] encode( char str[] ){
        char res[] = new char[str.length];

	for( int i=0; i<str.length; i++ ){
	    rotate();
	    res[i] = encode( str[i] );
	}
	return res;
    }

    public String encode( String s ){
        char a[] = s.toCharArray();

	char buf[] = encode( a );
	return new String( buf );
    }

    public void setState( byte a, byte b, byte c ){
        A.setState( a );
        B.setState( b );
        C.setState( c );
    }

    public void setState( String s ){
        char a[] = s.toCharArray();

        switch( a.length ){
	    default:
	    	C.setState( Letter.CharToLetter( a[2] ) );
		// fall through

	    case 2:
	    	B.setState( Letter.CharToLetter( a[1] ) );
		// fall through

	    case 1:
	    	A.setState( Letter.CharToLetter( a[0] ) );
		// fall through

	    case 0:
	    	break;
	}
    }
};
