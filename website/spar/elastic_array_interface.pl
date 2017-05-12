$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - the ElasticArray interface";
&generateHeader( $webpage, $title );
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
<p>
If a class wants to allow growing and shrinking like that of ordinary
arrays, it should implement the <code>ElasticArray</code> interface:        
<pre>
interface ElasticArray(| type t, int n |) extends Array(| t, n |)
{
    void setSize( [int^n] sz ) throws NegativeArraySizeException;
    int getRoom();                                                  
    void setRoom( int rm );                                         
    void setRoom( [int^n] rm );                               
    void fitRoom();              
}
</pre>
The main difference with normal arrays is that the user can specify a new
array size with the <code>setSize</code> method. This will grow or shrink
the array, while preserving elements that are visible before and after
the change of size.
<p>
For efficiency reasons it is possible to allocate more more elements to
an array than is necessary to store an array of the current size. The
user can request more room with the <code>setRoom</code> methods, query the
current room with the <code>getRoom</code> method, or release superfluous
room with the <code>fitRoom</code> method.
<p>
Standard Spar <a href="arrays.html">arrays</a> implement this interface,
but it can also be implemented by user-defined classes.
See the <a href="simple_array.html"><code>SimpleArray</code></a> class
definition for an example.
<p>
To demonstrate the use of the <code>ElasticArray</code> interface, the
function <code>listPositives</code> below returns a new array of appropriate
size that contains all positive elements of the given array.
<p>
Note that in this example the array grows one element a time. A real-life
function will try to be smarter than that by reserving room in the array
in advance.
<pre>
public class Demo
{
    public static double[] listPositives( double l[] )
    {
	double res[] = new double[0];
	int pos = 0;

	for( int ix=0; i&lt;l.size; ix++ ){
	    if( l[ix]&gt;0 ){
		res.setSize( [pos+1] );
		res[pos++] = l[ix];
	    }
	}
	return res;
    }

    static void main( String argv[] )
    {
	double arr[] = { 1, -1, 3, -66, 2, 0, -3, 2, 6 };

	System.out.println( "Result: "+listPositives( arr ) );
    }
}
</pre>
EOF
&generateFooter($0);
