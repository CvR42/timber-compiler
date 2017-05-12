// Should give the error message:
// inf_f() in err1 cannot implement inf_f() in inf; attempting to assign weaker access privileges; was public

interface inf {
    int inf_val = 8;
    int inf_f();
}

public final class err1 implements inf {
    int inf_f() { return 12; }
    public static void main( String args[] ){
	inf inf1 = new err1();
	System.out.println( "inf_val: " + inf1.inf_val + " inf_f(): " + inf1.inf_f() );
    }
}
