$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - tuples and vectors";
&generateHeader( $webpage, $title );
print "<p>\n";
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
</p>
<p>
Spar has extensive support for
<a href="arrays.html">arrays</a>, but this support is
designed for bulk operations on variable-length arrays. There are
circumstances where a more light-weight construct is appropriate. For
this reason Spar provides <dfn>vectors</dfn>. 
Vectors are important as indices into multi-dimensional arrays, in
particular in the parameterized classes that implement custom array
representations.
</p>
<p>
A vector type is written as the name of an element type, followed by a
Cartesian exponentiation operator (`<code>^</code>'), followed by a constant
expression. For example, the following are valid vector declarations:
</p>
<pre>
[int^3] x;
[double^2] y;
</pre>
<p>
Like primitive types, vectors do not have to be created explicitly,
and like primitive types, they are passed by value. A vector of
length 1 is <em>not</em> the same as its element.
</p>
<p>
Provided that the operations are defined on the elements, Spar
allows vector assignment, equality comparison, addition, subtraction,
and multiplication. Vector expressions can be written as a list of
values surrounded by square brackets. The code below demonstrates most
of these operations.
</p>
<pre>
[int^3] x;

x[0] = 1;           // Fill elements of the vector
x[1] = 2;
x[3] = 3;
x = [1,2,3];        // This is equivalent to the above
[int^3] y = x;  // Declaration with initialization
y += [1,0,0];       // Vector addition
y = y-x;            // Vector subtraction
y *= 2;             // Element-wise multiplication</pre>
EOF
&generateFooter($0);
