// File: ReflectorC.java
//
// Implements reflector `C' of the two possible reflectors in
// the naval Enigma.

public class ReflectorC extends Reflector {
    private static final byte encoder[] = {
	Letter.F, 	// from A
	Letter.V,	// B
	Letter.P,	// C
	Letter.J,	// D
	Letter.I,	// E
	Letter.A,	// F
	Letter.O,	// G
	Letter.Y,	// H
	Letter.E,	// I
	Letter.D,	// J
	Letter.R,	// K
	Letter.Z,	// L
	Letter.X,	// M
	Letter.W,	// N
	Letter.G,	// O
	Letter.C,	// P
	Letter.T,	// Q
	Letter.K,	// R
	Letter.U,	// S
	Letter.Q,	// T
	Letter.S,	// U
	Letter.B,	// V
	Letter.N,	// W
	Letter.M,	// X
	Letter.H,	// Y
	Letter.L 	// Z
    };

    byte encode( byte c ){
        return encoder[c];
    }
};
