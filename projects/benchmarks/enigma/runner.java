// File: runner.java
//
// Create an enigma machine, and run a number of letters through it.

public class runner {
    private static void verifyReflector( Reflector w )
    {
	for( byte c=0; c<Letter.ALPHABETH_SIZE; c++ ){
	    byte c1 = w.encode( c );
	    byte c2 = w.encode( c1 );
	    if( c2 != c ){
		System.out.println(
		    "Bad reflector encoding: " + 
		    Letter.LetterToChar( c ) + " -> " +
		    Letter.LetterToChar( c1 ) + " -> " +
		    Letter.LetterToChar( c2 )
		);
		System.exit( 1 );
	    }
	}
    }

    private static void verifyWheel( Wheel w )
    {
	for( byte state=0; state<Letter.ALPHABETH_SIZE; state++ ){
	    w.setState( state );
	    for( byte c=0; c<Letter.ALPHABETH_SIZE; c++ ){
		byte c1 = w.encodeLeft( c );
		byte c2 = w.encodeRight( c1 );
		if( c2 != c ){
		    System.out.println(
			"Bad wheel encoding (state " + state + "):" +
			Letter.LetterToChar( c ) + " -> " +
			Letter.LetterToChar( c1 ) + " -> " +
			Letter.LetterToChar( c2 )
		    );
		    System.exit( 1 );
		}
	    }
	}
    }


    private static boolean isPromising( String decode, String crib ){
        return decode.indexOf( crib ) >=0;
    }

    static void break_machine( Wheel A, Wheel B, Wheel C, String message, String crib )
    {
	Enigma machine = new Enigma( A, B, C, new ReflectorB() );
	for( byte a=0; a<Letter.ALPHABETH_SIZE; a++ ){
	    for( byte b=0; b<Letter.ALPHABETH_SIZE; b++ ){
		for( byte c=0; c<Letter.ALPHABETH_SIZE; c++ ){
		    machine.setState( a, b, c );
		    String decode = machine.encode( message );
		    if( isPromising( decode, crib ) ){
			System.out.println(
			    "Found promising decrypt for wheels " +
			    A.wheelName() + " " +
			    B.wheelName() + " " +
			    C.wheelName() + " " +
			    ", setting " +
			    Letter.LetterToChar( a ) +
			    Letter.LetterToChar( b ) +
			    Letter.LetterToChar( c )
			);
			System.out.println( "Decode is: " + decode );
		    }
		}
	    }
	}
    }

    public static void main( String args[] ){
	if( args.length>0 ){
	    if( args[0].equals( "encode" ) ){
		String message;
		String setting;
		Wheel A = new WheelI();
		Wheel B = new WheelII();
		Wheel C = new WheelIII();
		Reflector ref = new ReflectorB();
		Enigma machine = new Enigma( A, B, C, ref );

		if( args.length>1 ){
		    message = args[1];
		}
		else {
		    message = "Enigma cypher machine";
		}
		if( args.length>2 ){
		    setting = args[2];
		}
		else {
		    setting = "AKV";
		}
		System.out.println( "Running Enigma machine" );
		System.out.println( "Setting: "+setting );
		System.out.println( "Message: "+message );
		machine.setState( setting );
		String out1 = machine.encode( message );
		System.out.println( "Encoded: " + out1 );
		machine.setState( setting );
		String out2 = machine.encode( out1 );
		System.out.println( "Decoded again: " + out2 );
	    }
	    else if( args[0].equals( "test" ) ){
		System.out.println( "Verifying" );
		verifyReflector( new ReflectorB() );
		System.out.println( "Reflector B ok" );
		verifyReflector( new ReflectorC() );
		System.out.println( "Reflector C ok" );
		verifyWheel( new WheelI() );
		System.out.println( "Wheel I ok" );
		verifyWheel( new WheelII() );
		System.out.println( "Wheel II ok" );
		verifyWheel( new WheelIII() );
		System.out.println( "Wheel III ok" );
		Wheel A = new WheelI();
		Wheel B = new WheelII();
		Wheel C = new WheelIII();
		Reflector ref = new ReflectorB();
		Enigma machine = new Enigma( A, B, C, ref );
		for(;;){
		    if( machine.rotate() ){
			// We've traversed the entire state space
			// of the machine.
			break;
		    }
		    for( byte c=0; c<Letter.ALPHABETH_SIZE; c++ ){
			byte c1 = machine.encode( c );
			byte c2 = machine.encode( c1 );
			if( c2 != c ){
			    System.out.println(
				"Bad encoding: " + 
				Letter.LetterToChar( c ) + " -> " +
				Letter.LetterToChar( c1 ) + " -> " +
				Letter.LetterToChar( c2 )
			    );
			    System.exit( 1 );
			}
		    }
		}
		System.out.println( "Done" );
	    }
	    else if( args[0].equals( "break" ) ){
		String message;
		String crib;
		Wheel A = new WheelI();
		Wheel B = new WheelII();
		Wheel C = new WheelIII();

		if( args.length>1 ){
		    message = args[1];
		}
		else {
		    message = "Enigma cypher machine";
		}
		if( args.length>2 ){
		    crib = args[2];
		}
		else {
		    crib = "CYPHER";
		}
		for( int n=0; n<10; n++ ){
		    break_machine( A, C, B, message, crib );
		    break_machine( A, B, C, message, crib );
		    break_machine( B, A, C, message, crib );
		    break_machine( B, C, A, message, crib );
		    break_machine( C, B, A, message, crib );
		    break_machine( C, A, B, message, crib );
		}
	    }
	    else {
	        System.out.println( "Command must be one of:" );
	        System.out.println( "test\t\tRun exhaustive test" );
	        System.out.println( "encode <string> [<setting>]\tEncode a string" );
	        System.out.println( "break <string> [<crib>]\tBreak a cypher" );
	    }
	}
	else {
	    int sum = 0;
	    Wheel A = new WheelI();
	    Wheel B = new WheelII();
	    Wheel C = new WheelIII();
	    Reflector ref = new ReflectorB();
	    Enigma machine = new Enigma( A, B, C, ref );

	    for( int setting=0; setting<60; setting++ ){
		for( int i=0; i<26*26*26; i++ ){
		    machine.rotate();
		    sum += machine.encode( (byte) 0 );
		}
	    }
	    System.out.println( sum );
	}
    }
};
