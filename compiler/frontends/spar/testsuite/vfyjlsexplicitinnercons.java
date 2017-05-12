// After JLS2 8.8.5.1    Explicit Constructor Invocations


class Outer {
    class Inner{}
}
class ChildOfInner extends Outer.Inner {
    ChildOfInner(){(new Outer()).super();}
}

public class vfyjlsexplicitinnercons {
    public static void main( String args[] ){
	Outer x = new Outer();
	ChildOfInner y = new ChildOfInner();
    }
}
