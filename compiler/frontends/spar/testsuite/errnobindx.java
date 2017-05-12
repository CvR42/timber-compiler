// Test of a binding restriction that also applies to
// the standard Java compiler: a local class cannot refer to fields of
// an enclosing class.

interface superinf {
    int superinf_val = 3;
    int superinf_f();
}

interface inf extends superinf {
    int inf_val = 8;
    int inf_f();
}

class cl implements inf {
    public int inf_f() { return 12; }
    public int superinf_f() { return 42; }
}

public final class errnobindx {
    cl x = new cl();
    void doit() {
	inf inf1 = new inf(){
	    // Error: 'x' below cannot be bound.
	    public int inf_f() { return x.inf_f(); }
	    public int superinf_f() { return x.super.inf_f(); }
	};
	System.out.println( "inf_f(): "+ inf1.inf_f() );
	System.out.println( "superinf_f(): "+ inf1.superinf_f() );
    }
    public static void main(){
	new errnobindx().doit();
    }
}
