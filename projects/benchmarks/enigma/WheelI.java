// File: WheelI.java
//
// Implements Wheel 'I' of the Enigma wheel set.

class WheelI extends Wheel {
    static final byte encoder[] = {
	Letter.E,	// from A
	Letter.K,	// B
	Letter.M,	// C
	Letter.F,	// D
	Letter.L,	// E
	Letter.G,	// F
	Letter.D,	// G
	Letter.Q,	// H
	Letter.V,	// I
	Letter.Z,	// J
	Letter.N,	// K
	Letter.T,	// L
	Letter.O,	// M
	Letter.W,	// N
	Letter.Y,	// O
	Letter.H,	// P
	Letter.X,	// Q
	Letter.U,	// R
	Letter.S,	// S
	Letter.P,	// T
	Letter.A,	// U
	Letter.I,	// V
	Letter.B,	// W
	Letter.R,	// X
	Letter.C,	// Y
	Letter.J	// Z
    };
    static final byte reverse_encoder[] = buildRightTable( encoder );
	       // Y        Q

    public byte encodeLeft( byte c ){
	int ix = c-state;
	if( ix<0 ){
	    ix += Letter.ALPHABETH_SIZE;
	}
        return (byte) ((encoder[ix]+state)%Letter.ALPHABETH_SIZE);
    }

    public byte encodeRight( byte c ){
	int ix = c-state;
	if( ix<0 ){
	    ix += Letter.ALPHABETH_SIZE;
	}
        return (byte) ((reverse_encoder[ix]+state)%Letter.ALPHABETH_SIZE);
    }

    public boolean rotate(){
	boolean rotate_next = state == Letter.Q;
        state++;
	if( state>=Letter.ALPHABETH_SIZE ){
	    state -= Letter.ALPHABETH_SIZE;
	}
	return rotate_next;
    }

    public String wheelName(){ return "I"; }
};
