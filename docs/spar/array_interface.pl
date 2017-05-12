$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - the Array interface";
&generateHeader( $webpage, $title );
print <<EOF;
<table>
<tr>
<td>
EOF
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
<p>
To support sparse matrices and other specialized array implementations,
Spar allows any class that implements the <code>Array</code> interface
to use the array access syntax.
</table>
<h2>Definition</h2>
The <code>Array</code> interface is defined as follows
(note that the interface implements
<a href="arrays.html">multi-dimensional arrays</a>, and uses <a href="vectors.html">vectors</a> and
<a href="templates.html">parameterized types</a>):
<pre>
interface Array(| type t, int n |)
{
    t getElement( [int^n] index )
        throws IndexOutOfBoundsException;
    void storeElement( [int^n] index, t elm )
        throws IndexOutOfBoundsException,
               ArrayStoreException;
    [int^n] getSize();
}
</pre>
<p>
If a class wants to allow growing and shrinking of its
arrays, it should implement the <code>ElasticArray</code> interface:
<pre>
interface ElasticArray(| type t, int n |)
    extends Array(| t, n |)
{
    void setSize( [int^n] sz )
        throws NegativeArraySizeException;
    int getRoom();
    void setRoom( int rm );
    void setRoom( [int^n] rm );
    void fitRoom();
}
</pre>
<h2>Example - diagonal view</h2>
The following class defines a `view' on the diagonal of
a two-dimensional array:
<pre>
class DiagonalView(| type t |) implements Array(| t, 1 |)
{
    Array(| t, 2 |) ref;
<p>
    DiagonalView( Array(| t, 2 |) a ){ ref = a; }
    t getElement( [int^1] ix ){
        return ref[ix[0],ix[0]];
    }
    void storeElement( [int^1] ix, t elm ) {
        ref[ix[0],ix[0]] = elm;
    }
    [int^1] getSize() {
        [int^2] dims = ref.getSize();
        return [Math.min( dims[0], dims[1] )];
    }
}
</pre>
This class can now be used as follows:
<pre>
int[*,*] a = new double[5,5];
DiagonalView(| double |) v = DiagonalView(| double |)( a );
a[:,:] = 0;
v[:] = 1;
</pre>
This will construct and fill `<code>a</code>' with the identity matrix
(1 on the diagonal, 0 elsewhere).
<h2>Example - transpose view</h2>
As another example, the following class 
implements a transpose view on an array of arbitrary size.
<pre>
class TransposeView(| type t, int n |)
    implements Array(| t, n |)
{
    Array(| t, n |) ref;

    TransposeView( Array(| t, n |) a ){ ref = a; }
    t getElement( [int^n] ix ){
        return ref revVector( n, ix );
    }
    void storeElement( [int^n] ix, t elm ){
        ref revVector( n, ix ) = elm;
    }
    [int^n] getSize(){
        return revVector( n, ref.getSize() );
    }
    static [int^n] revVector( int n, [int^n] v ){
        int ix;
        [int^n] res;

        macro for( ix :- 0:n; ix++ ){
            res[(n-ix)-1] = v[ix];
        }
        return res;
    }
}
</pre>
Note that this class even works for 0-dimensional and 1-dimensional
arrays.  Also note that since this class allows a view on an arbitrary
<code>Array</code> class it is possible to <dfn>compose</dfn> views. 
This is demonstrated below, where two transpose views are composed.
<p>
This could be used as follows:
<pre>
int[*,*] a = new double[5,5];
TransposeView(|double|) v = TransposeView(|double|)( a );
TransposeView(|double|) w = TransposeView(|double|)( v );
a[:,:] = 0;
v[:,0] = 1;
w[:,0] = 2;
</pre>
This results in a matrix `<code>a</code>' where all elements are set to 0,
except that the first <em>column</em> (apart from <code>a[0,0]</code>) is set
to 1 through the transpose view `<code>v</code>` on `<code>a</code>', and the
first <em>row</em> is set to 2 through the transpose view `<code>w</code>' on
`<code>v</code>'. Note that since `<code>w</code>' is a transpose view on a
transpose view, it is in effect an identity view.
EOF
&generateFooter($0);
