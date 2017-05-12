$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - array expressions";
&generateHeader( $webpage, $title );
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
An array expression is a shorthand notation for the construction of a
(partial) copy of an array. For example, the following code will
first construct an array <code>a</code>, and then construct a copy of the
first row of <code>a</code>, and assign it to <code>v</code>:
<p>
<table>
<tr>
<td>
<pre>
int[*,*] a = {{0,1,2},{3,4,5},{6,7,8}};
int[*] v = a[0,0:a.getSize(1)];
</pre>
</table>
<p>
Note that <code>v</code> is a <em>copy</em>, so subsequent assignments to elements
of <code>v</code> are not visible in <code>a</code>. Contrary to array range notations
in many other languages, the top of the specified range is the first element
<em>not</em> to be included.
<p>
The usual range shorthands apply: if no start of the range is given,
0 is assumed, and if no end of the range is given, the size in that
dimension is assumed. Thus the declaration of <code>v</code> in the previous
code fragment could be written as:
<pre>
int[*] v = a[0,:];
</pre>
<p>
<h2>Array statements</h2>
Array statements are a shorthand notation for a <code>foreach</code>
statement that is executed for all elements of a selected range. For
example, the code fragment
<pre>
Block[*,*] a = new Block[5,7];
a[:,:].init();
</pre>
will invoke the method <code>init</code> on all elements of <code>Block</code>
array <code>a</code>. Since this is equivalent to a \term{foreach} statement, the
<code>init</code> method of each of the array elements is not invoked in a
prescribed order.
<p>
Similarly, array assignments are a shorthand for repeated
assignments. For example:
<pre>
int[*,*] a = new int[5,7];
<p>
a[:,:] = 0;
</pre>
will zero the entire array <code>a</code>.
<p>
The expression at the right-hand side of the assignment will be evaluated
only once. Thus,
<pre>
int ix = 0;
int[*,*] a = new int[5,7];
<p>
a[:,:] = ix++;
</pre>
will again zero the entire array <code>a</code>, and will leave <code>ix</code> with
the value 1.
<p>
Last but not least, array assignments may contain an array at the
right-hand side, instead of a single element. In that case every iteration
of the <code>foreach</code> will use the implicit iteration vector as index
for every assignment.
<p>
For example:
<pre>
int[*,*] a = new int[5,7];
int[*,*] b = new int a.getSize();
<p>
b[:,:] = 1;
a[:,:] = b;
</pre>
will copy <code>b</code> into <code>a</code>. The last statement could also be written
as:
<pre>
a[:,:] = b[:,:];
</pre>
but a naive compiler would first create a copy of <code>b</code>, and leave it
for the garbage collector.
<p>
Array statements never change the size of the array they work on. Any
access that is out of bounds is detected at compile-time or run-time,
and causes an error message or an <code>IndexOutOfBoundsException</code>
exception.
EOF
&generateFooter($0);
