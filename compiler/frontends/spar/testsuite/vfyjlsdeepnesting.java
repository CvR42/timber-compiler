// After JLS2 8.1.2    Inner Classes and Enclosing Instances



class DeepNesting {
    boolean toBe;
    DeepNesting(boolean b) { toBe = b;}
    class Nested {
        boolean theQuestion;
        class DeeplyNested {
            DeeplyNested(){
                theQuestion = toBe || !toBe;
            }
        }
    }
}

public class vfyjlsdeepnesting {
    public static void main( String args[] ){
	DeepNesting v = new DeepNesting( true );
    }
}
