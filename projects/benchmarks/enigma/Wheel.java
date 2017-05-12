// File: Wheel.java
//
// An abstract Enigma wheel.

abstract class Wheel {
    protected byte state;	// The state of the Wheel.

    public abstract byte encodeLeft( byte c );
    public abstract byte encodeRight( byte c );
    public abstract boolean rotate();
    public abstract String wheelName();

    public byte getState() { return state; }
    public void setState( byte st ) { state = (byte) (st % Letter.ALPHABETH_SIZE); }

    // Helper function. Given an encoding table implementing
    // the Left encoding, return a table implementing the Right encoding.
    static final byte[] buildRightTable( final byte[] table ) {
        byte res[] = new byte[table.length];

	for( int i=0; i<table.length; i++ ){
	    res[table[i]] = (byte) i;
	}
	return res;
    }
}
