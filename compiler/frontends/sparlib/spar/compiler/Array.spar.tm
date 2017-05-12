// File: Array.spar
//
// Helper functions for arrays. In particular, the clone() and toString()
// methods for arrays are implemented here.

package spar.compiler;

.. The ranks that we support.
.set ranks 0 1 2 3 4 5 6 7 8
..
public final class Array {
    private Array() { /* this class is not instantiable by the general public */ }    

.foreach t char byte boolean int long float double complex 
.foreach rank $(ranks)
.set formalpat *^$(rank)
    // Cloning function for type $t[$(formalpat)]
    <$$ nullvalue=false $$> public static $t[$(formalpat)] arrayClone( final $t arr[$(formalpat)] )
    throws CloneNotSupportedException
    {
        char res[$(formalpat)] = new $t@(arr.getSize());

	for( v :- :arr.getSize() ){
	    res@v = arr@v;
	}
	return res;
    }

.endforeach
.endforeach
.foreach rank $(ranks)
.set formalpat *^$(rank)
    // Cloning function for Object[$(formalpat)]
    <$$ nullvalue=false $$> public static Object[] arrayClone( final Object arr[$(formalpat)] )
    {
        char res[$(formalpat)] = new Object@(arr.getSize());

	for( v :- :arr.length ){
	    res@v = (arr@v).clone();
	}
	return res;
    }

.endforeach
.. toString functions
.foreach t char byte boolean int long float double complex 
.foreach rank $(ranks)
.set formalpat *^$(rank)
    // Cloning function for type $t[$(formalpat)]
    <$$ nullvalue=false $$> public static String arrayToString( final $t arr[$(formalpat)] )
    {
.if $[$(rank)==1]
	return "[C@" + arr.hashCode();
.else
	return "[$(rank)C@" + arr.hashCode();
.endif
    }

.endforeach
.endforeach
.foreach rank $(ranks)
.set formalpat *^$(rank)
    // Cloning function for Object[$(formalpat)]
    <$$ nullvalue=false $$> public static String arrayToString( final Object arr[$(formalpat)] )
    {
.if $[$(rank)==1]
	return "[C@" + arr.hashCode();
.else
	return "[$(rank)C@" + arr.hashCode();
.endif
    }

.endforeach
}
