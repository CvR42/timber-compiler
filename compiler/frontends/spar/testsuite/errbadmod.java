// File: errbadmod.java
//
// This tests JLS 8.1.1: "The access modifiers 'protected' or 'private'
// pertain only to member classes with a directly enclosing class
// declaration".
// It also tests JLS 9.1.1, with the equivalent restriction.

protected class t1 {}		// Error
private class t2 {}		// Error
private interface inf1 {}	// Error
protected interface infw {}	// Error

public class errbadmod {
    protected class t3{}
    private class t4{}
    static void main( String args[] ){
	protected class t5{};
	protected class t6{};
	int n;
	n = 3;
    }
}
