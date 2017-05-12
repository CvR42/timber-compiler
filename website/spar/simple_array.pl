$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - an example: the SimpleArray class";
&generateHeader( $webpage, $title );
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
<p>
To demonstrate the implementation of the <a
href="elastic_array_interface.html"><code>ElasticArray</code></a>
interface, and special array representations in general, the following
class implements a parameterized class <code>SimpleArray</code> that
provides the same functionality as the built-in arrays, but maps them to
fictional <code>malloc</code>, <code>realloc</code> and <code>free</code>
 function calls.
See the pages on <a href="macros.html">inlining</a>,
<a href="templates.html">parameterized classes</a>, and <a
href="arrays.html">multidimensional arrays</a> for an explanation of
the used language constructs.
<pre>
class SimpleArray( type t, int n )
    implements ElasticArray( t, n )
{
    t[*] mem;
    [int^n] size;
    int room;

    // Some helper methods.
    inline static private int vecProd( [int^n] v )
    {
        int res = 1;

        inline for( ix :- 0:n ) {
            res *= v[ix];
        }
        return res;
    }

    inline private int calcElement( int^n v )
    {
        int res = 0;
        int w = 1;

        for( ix :- 0:n ){
            res += w*v[ix];
            w *= size[ix];
        }
        return res;
    }

    // The constructor for this array
    SimpleArray( [int^n] sz )
        throws NegativeArraySizeException
    {
        room = 0;
        size[:] = 0;
        mem = null;
        setSize( sz );
    }

    inline t getElement( [int^n] index )
        throws IndexOutOfBoundsException
    {
        int ix = calcElement( index );
        return mem[ix];
    }

    inline void storeElement( [int^n] index, t elm )
        throws IndexOutOfBoundsException,
               ArrayStoreException;
    {
        int ix = calcElement( index );
        mem[ix] = elm;
    }

    inline [int^n] getSize() { return size; }

    void setSize( [int^n] sz )
        throws NegativeArraySizeException
    {
        setRoom( sz );
        if( vecProd( sz )&lt;0 ){
            throw new NegativeArraySizeException();
        }
        size = sz;
    }

    inline int getRoom(){ return room; }

    void setRoom( int rm )
    {
        if( rm&lt;room ){
            return;
        }
        // We assume malloc throws an OutOfMemoryError
        // if necessary.
        if( t == null )
            mem = malloc( t, rm );
        else {
            mem = realloc( mem, t, rm );
            // Some fairly difficult reshuffling goes here.
        }
        room = rm;
    }

    inline void setRoom( int^n sz ){ setRoom( vecProd( sz ); }

    void fitRoom()
    {
        int rm = vecProd( sz );
        if( mem != null )
            // As if it really shrinks. Hah!
            mem = realloc( mem, t, rm );
        room = rm;
    }

    // This method is invoked when the array is destroyed.
    void finalize()
    {
        if( mem != null )
            free( mem );
    }
}
</pre>
EOF
&generateFooter($0);
