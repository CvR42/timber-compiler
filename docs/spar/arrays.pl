$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - arrays";
&generateHeader( $webpage, $title );
print "<p>\n";
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
</p>
<p>
An array type is written as the name of an element type, followed by a
number of abstract shape specifications. For example:
</p>
<pre>
int[*] v;               // A 1-dimensional array
int[*,*] A;             // A 2-dimensional array
</pre>
<p>
For compatibility with Java, Spar also allows Java-style declarations
of one-dimensional arrays:
</p>
<pre>
int[] n;                // A 1-dimensional array
</pre>
<p>
This is a special case.
</p>
<p>
Here are some examples of declarations of array variables that create
array objects:
</p>
<pre>
int a[*] = new int[4];
short b[*,*] = new short[6,8];
int sq[*] = { 1, 4, 9, 16, 25, 36 };
real ident[*,*] = {{1,0,0}, {0,1,0}, {0,0,1}};
real vv[*][*] = {{1,0,0}, {0,1,0}, {0,0,1}};
String[] aos = { "array", "of", "string" };
</pre>
<p>
Note that <code>ident</code> and <code>vv</code> have the same initialization
expression, but they are <em>not</em> equivalent. The first is a
two-dimensional array, the second is a one-dimensional array of
one-dimensional arrays.
</p>
<p>
A component of an array is accessed by an expression
that consists of an array reference followed by an <code>int</code>
<a href="vectors.html">vector</a>
expression, as in: <code>A[i,j]</code>. All arrays start at element 0. A
one-dimensional array with length <var>n</var> can be indexed by the integers 0
to <var>n-1</var>.
</p>
<p>
For example, the following assigns 2 to array element <code>[2,3]</code> of
array <code>A</code>.
</p>
<pre>
int[*,*] A = new int[9,9]; // A 2-dim array
A[2,3] = 2;                // An array access
</pre>
<p>
This looks very similar to array access in other languages, but in the case
of Spar this is a special case of a more general access construct, where
arbitrary
<a href="vectors.html">vector</a>
expressions can be used to access an array.
Since any
<a href="vectors.html">vector</a>
expression can be used, very
powerful access expressions are possible. This is demonstrated in the
following code:
</p>
<pre>
int[*,*] A = new int[9,9];  // A 2-dim array
[int^2] v = [1,1];          // A vector of 2 elm.
A\@v = 3;                   // A[1,1] = 3
A\@(3*v) = 4;               // A[3,3] = 4 </pre>
<p>
As much as possible, array accesses are checked for bounds
violations at compile time.  If the array access cannot be
checked at compile time, an index that is out of bounds causes an
<code>IndexOutOfBoundsException</code> to be thrown.
</p>
EOF
&generateFooter($0);
