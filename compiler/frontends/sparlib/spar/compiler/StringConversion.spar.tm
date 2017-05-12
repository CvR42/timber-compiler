// File: StringConversion.spar
//
// Helper functions for string conversion.
//
// This class contains a series of of overloaded methods `convert' that
// get a reference type (i.e. an Object subclass, including arrays) instance.
//
// These methods implement the string conversion described in JLS2 15.18.1.1:
// if the reference is null, return the string "null", else invoke the
// toString() method of the class. If that returns null, return the
// string "null", else return the result of the toString() invocation.
//
// All 'normal' classes are handled by convert( Object ), remainder of
// the methods are used to handle string conversion of arrays.
// The array methods do not invoke the appropriate toString() method, but
// directly implement what that method would have returned.

package spar.compiler;

.. The array ranks that we support.
.set ranks 0 1 2 3 4 5 6 7 8
..
public final class StringConversion {
    private StringConversion() { /* this class is not instantiable by the general public */ }    

.. toString functions
.foreach t char byte boolean int long float double complex 
.foreach rank $(ranks)
.set formalpat *^$(rank)
    // String conversion function for type $t[$(formalpat)]
    <$$ nullvalue=false $$> public static String convert( final $t arr[$(formalpat)] )
    {
        if( arr == null ){
	    return "null";
	}
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
    // String conversion function for Object[$(formalpat)]
    <$$ nullvalue=false $$> public static String convert( final Object arr[$(formalpat)] )
    {
        if( arr == null ){
	    return "null";
	}
.if $[$(rank)==1]
	return "[C@" + arr.hashCode();
.else
	return "[$(rank)C@" + arr.hashCode();
.endif
    }

.endforeach
    // String conversion function for Object
    <$$ nullvalue=false $$> public static String convert( final Object obj )
    {
        if( obj == null ){
	    return "null";
	}
	String res = obj.toString();
	if( res == null ){
	    return "null";
	}
	return res;
    }

}
