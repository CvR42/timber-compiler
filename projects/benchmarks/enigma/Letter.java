// File: Letter.java
//
// Methods and constants used for Enigma character representation.

class Letter {
    static final byte A = 0;
    static final byte B = 1;
    static final byte C = 2;
    static final byte D = 3;
    static final byte E = 4;
    static final byte F = 5;
    static final byte G = 6;
    static final byte H = 7;
    static final byte I = 8;
    static final byte J = 9;
    static final byte K = 10;
    static final byte L = 11;
    static final byte M = 12;
    static final byte N = 13;
    static final byte O = 14;
    static final byte P = 15;
    static final byte Q = 16;
    static final byte R = 17;
    static final byte S = 18;
    static final byte T = 19;
    static final byte U = 20;
    static final byte V = 21;
    static final byte W = 22;
    static final byte X = 23;
    static final byte Y = 24;
    static final byte Z = 25;
    static final int ALPHABETH_SIZE = 26;

    static byte CharToLetter( char c ){
        switch( c ){
	    case 'a':	case 'A':	return A;
	    case 'b':	case 'B':	return B;
	    case 'c':	case 'C':	return C;
	    case 'd':	case 'D':	return D;
	    case 'e':	case 'E':	return E;
	    case 'f':	case 'F':	return F;
	    case 'g':	case 'G':	return G;
	    case 'h':	case 'H':	return H;
	    case 'i':	case 'I':	return I;
	    case 'j':	case 'J':	return J;
	    case 'k':	case 'K':	return K;
	    case 'l':	case 'L':	return L;
	    case 'm':	case 'M':	return M;
	    case 'n':	case 'N':	return N;
	    case 'o':	case 'O':	return O;
	    case 'p':	case 'P':	return P;
	    case 'q':	case 'Q':	return Q;
	    case 'r':	case 'R':	return R;
	    case 's':	case 'S':	return S;
	    case 't':	case 'T':	return T;
	    case 'u':	case 'U':	return U;
	    case 'v':	case 'V':	return V;
	    case 'w':	case 'W':	return W;
	    case 'x':	case 'X':	return X;
	    case 'y':	case 'Y':	return Y;
	    case 'z':	case 'Z':	return Z;
	    case '\'': case '`':	return Q;
	}
	return X;
    }

    static char LetterToChar( byte c ){
	if( c<ALPHABETH_SIZE ){
	    return (char) ('A'+c);
	}
	return 'X';
    }
}
