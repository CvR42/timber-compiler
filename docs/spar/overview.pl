$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - overview";
$automap_url = &generate_relative_url( $webpage, "automap/index.html" );
&generateHeader( $webpage, $title );
print <<EOF;
<p>
<em>Spar</em> is an experimental language that is part of the <a href="$automap_url">Automap</a>
project. Its main goals are:
</p>
<ul>
<li>Convenient <em>parallel programming</em>.</li>
<li>Support for <em>multi-dimensional</em> arrays.</li>
<li>Support for <em>sparse</em> arrays.</li>
</ul>
<p>
to meet these goals a number of language constructs have been designed.
For practical reasons we have chosen to embed them in the
programming language <a href="http://java.sun.com">Java</a>.
</p>
<p>
Although not originally intended as such, Spar is almost a superset of Java.
See our <a href="sins.html">`sins'</a> page for an overview of the
deviations, and the reasons for them.
</p>
<p>
A native-code compiler is under construction. A working prototype is
expected shortly.  No attempt is made to use the JVM.
</p>

<h2>Parallelising constructs</h2>
<p>
The parallelising constructs have been designed to make them
as safe as possible, while still allowing a compiler
to derive opportunities for parallelism from them.
For this purpose the <code>each</code> and <code>foreach</code>
commands are provided.
</p>
<p>
Given a program block such as:
</p>
<pre>
each { s1; s2; }
</pre>
<p>
The statements <code>s1</code> and <code>s2</code> are executed in
arbitrary order. It is guaranteed, even for compound statements, that both
<code>s1</code> and <code>s2</code> are executed as one state transition.
</p>
<p>
The <code>foreach</code> has similar behavior for all its iterations. E.g.
</p>
<pre>
foreach( i :- 0:n ) a[i].init();
</pre>
<p>
<a href="foreach.html">elaboration</a>
</p>
<h2>Parameterised classes and interfaces</h2>
<p>
Parameterisation of classes, similar to C++ templates, is supported.
Interfaces can also be parameterised.
</p>
<p>
<a href="templates.html">elaboration</a>
</p>

<h2>Vectors and tuples</h2>
<p>
Tuples are light-weight datastructures for groups
of elements of fixed size. E.g.
</p>
<pre>
[int,char] x = [1,'c'];
int n = x[0]+2;
char c = x[1];
x = [12,'z'];
[n,c] = x; </pre>
<p>
If all the elements of the tuple have the same type, the tuple type can
be written with a shorthand.
</p>
<pre>
[int^3] x;

x[0] = 1;     // Fill element
x = [1,2,3];  // Fill entire tuple
</pre>
<p>
Such a tuple is sometimes called a <em>vector</em>.
</p>
<p>
If the operation is defined on the elements of a tuple, it is also
defined on an entire tuple. E.g.
</p>
<pre>
x += [1,0,0]; // Vector addition
</pre>

<p>
All array accesses use vectors. E.g., this assigns 4 to <code>a[0,0]</code>:
</p>
<pre>
[int^2] v = [0,0];
a\@v = 4;
</pre>
<p>
<a href="vectors.html">elaboration</a>
</p>
<h2>Macros</h2>

<p>
Macros are similar to the <code>inline</code> statement of C++, but
they are <em>always</em> obeyed. Also, macros can be given
types as parameters.
</p>
<p>
<a href="macros.html">elaboration</a>
</p>

<h2>Arrays</h2>
<ul>
<li>Elastic: can be grown and shrunk during their life.</li>
<li>Any class implementing the <code>Array</code>
interface enjoys the priviledged notation of arrays.</li>
<li>Support for array sections. E.g. <code>A[3:5,:]</code>.</li>
</ul>
<p>
<a href="arrays.html">elaboration of arrays</a><br />
<a href="array_expressions.html">elaboration of array expressions</a><br />
<a href="array_interface.html">elaboration of the <code>Array</code> interface</a><br />
<a href="elastic_array_interface.html">elaboration of the <code>ElasticArray</code> interface</a><br />
</p>

<h2>Restrictions</h2>
<ul>
<li>Threads are not supported.</li>
<li>Some libraries are not supported.</li>
<li>Dynamic class loading is not supported.</li>
</ul>
<p>
Dynamic class loading is currently impossible because Spar is a batch compiler.
The other restrictions are only determined by the implementation effort,
not by a fundamental obstacle.
</p>
EOF
&generateFooter($0);
