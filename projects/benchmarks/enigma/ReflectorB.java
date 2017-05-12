// File: ReflectorB.java
//
// Implements reflector `B' of the two possible reflectors in
// the naval Enigma.

public class ReflectorB extends Reflector {
    private static final byte encoder[] = {
	Letter.Y, 	// from A
	Letter.R,	// B
	Letter.U,	// C
	Letter.H,	// D
	Letter.Q,	// E
	Letter.S,	// F
	Letter.L,	// G
	Letter.D,	// H
	Letter.P,	// I
	Letter.X,	// J
	Letter.N,	// K
	Letter.G,	// L
	Letter.O,	// M
	Letter.K,	// N
	Letter.M,	// O
	Letter.I,	// P
	Letter.E,	// Q
	Letter.B,	// R
	Letter.F,	// S
	Letter.Z,	// T
	Letter.C,	// U
	Letter.W,	// V
	Letter.V,	// W
	Letter.J,	// X
	Letter.A,	// Y
	Letter.T 	// Z
    };

    byte encode( byte c ){
        return encoder[c];
    }
};
