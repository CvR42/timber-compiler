// Test: only member interfaces can be declared static. (JLS2 9.1.1)

static interface inf {
}

public class errstaticinf implements inf {
    public static void main(){
	int n;
	n = 3;
    }
}
