interface superinf {
    int superinf_val = 3;
    int superinf_f();
}

interface inf extends superinf {
    int inf_val = 8;
    int inf_f();
}

// Error: private not allowed here
private class cl implements inf {
    int inf_f() { return 12; }
    int superinf_f() { return 42; }
}

public final class errprivateouterclass {
    public static void main( String args[] ){
	inf inf1 = new cl();
	System.out.println( "inf_val: " + inf1.inf_val + " inf_f(): " + inf1.inf_f() );
    }
}
