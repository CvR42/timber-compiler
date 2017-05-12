$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - parameterized types";
&generateHeader( $webpage, $title );
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
Spar generalizes Java classes by allowing <dfn>parameterization</dfn>.
For example, the <a href="http://java.sun.com/docs/books/tutorial/index.html">Java tutorial</a> shows
a stack class that can hold elements of arbitrary type. If we wanted
to restrict the stack to elements of a given type, we could implement
a separate class for each type, but it is much more useful to implement
a generic, parameterized, stack.  In Spar this is possible as follows:
<pre>
class TypedStack(| type t |) {
    static final int STACK_EMPTY = -1;
    t[] stackelements;
    int topelement = STACK_EMPTY;

    void push( t e ){
        stackelements[++topelement] = e;
    }
    t pop() {
        return stackelements[topelement--];
    }
    boolean isEmpty(){
        return ( topelement == STACK_EMPTY );
    }
}
</pre>
An instance of this class could be used as follows:
<pre>
TypedStack(|char|) s = new TypedStack(|char|)();
<p>
s.push( 'a' );
s.push( 'b' );
char c = s.pop();
</pre>
Parameterization is not restricted to type parameters.
In Spar, parameterized classes are always expanded at compile-time.
<p>
<h2>Interfaces</h2>
Just like classes, Spar interfaces can be parameterized:
<pre>
interface Collection(| type t |){
    void add( t obj );
    void delete( t obj );
    t find( t obj );
    int currentCount();
}
</pre>
A class can inherit this interface as follows:
<pre>
Class Bag(| type t |) implements Collection(| type t |) {
    . . .
};
</pre>
An important use of parameterized interfaces is the
<a href="array_interface.html"><code>Array</code> interface</a>.
EOF
&generateFooter($0);
