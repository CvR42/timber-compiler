// File: .spar
//
// Test for the existence of communication routines with the GC.

class Thing {
    Thing next;
    Thing( Thing nxt ) { next = nxt; }
}

public final class t {
    public static void main(){
	Thing chain = null;

	spar.compiler.GC.setMemoryLimit( 1000000 );
	for(;;){
	    chain = new Thing( chain );
	}
    }
}
