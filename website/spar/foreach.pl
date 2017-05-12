$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - parallelization constructs";
&generateHeader( $webpage, $title );
print "<p>\n";
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
</p>
<h2>The parallelization model</h2>
<p>
Using parallel constructs, the user divides the program into a
number of medium-grain tasks, and the compiler automatically maps
the code and data onto a distributed-memory system. The mapping is
chosen to minimize the communication between processors, and maximize
the utilization of the processors.
</p>
<p>
To simplify the cost estimation and scheduling mechanisms required for
automatic mapping, <a href="../automap/index.html">Automap</a> currently
restricts programs to a class conforming to the
<a href="http://rama.pds.twi.tudelft.nl/~gemund/SPC/spc.html">SPC</a>
(Series-Parallel Contention) programming model. Clearly, there will be
loss of parallelism
due to this restriction, but this has been conjectured to be small.
Compelling evidence in support of this claim is provided in
the paper ``<a href="ftp://dutepp0.et.tudelft.nl/pub/gemund/p-euro-pds97.ps.Z">On the Loss of Parallelism by Imposing Synchronization Structure</a>''.
Nevertheless, future versions of the Spar compiler may lift this
restriction (if only to support Java threads).
</p>
<h2>Design considerations</h2>
<p>
Ideally, parallelization would be handled by the compiler without concerning
of the programmer, but until now attempts to
make such compilers have not been successful.
In
<a href="../automap/index.html">Automap</a>
we accept that language constructs for parallelization are necessary, 
but we try to make them as accessible as possible.
This requires more work by the compiler, and possibly the resulting
programs will be less efficient, but we assume that these negative
effects are limited.
The costs we incur are comparable in nature to the costs of high-level
languages compared to hand-written assembly programs. Often 
these costs are
an acceptable tradeoff for the shorter program development
time.
</p>
<p>
Another advantage of our abstract parallelization constructs is that they are more
likely to be portable, since the details of program optimization for a
given machine are handled by the compiler. Again, there is a clear
analogy with high-level programming languages.
</p>
<p>
In Spar we provide the <code>foreach</code> construct, which specifies that
the iterations can be executed in arbitrary order, but that each iteration
is to be executed sequentially. This model is fairly easy to understand for
the programmer, and requires less complicated analysis by the compiler than
analyzing strict sequential <code>for</code> loops. In principle the semantics
are the same for sequential or parallel execution. Moreover, reduction
operations can be formulated quite elegantly, which is not possible with
the <code>forall</code> or with explicitly parallel loops.
</p>
<h2>The language constructs</h2>
<p>
Given a block such as:
</p>
<pre>
each { s1; s2; } </pre>
<p>
The statements <code>s1</code> and <code>s2</code> are executed in
arbitrary order. It is guaranteed, even for compound statements, that
every statement in the <code>each</code> block is executed sequencially.
Thus, in this example it will look to the programmer as if the compiler chooses one of the execution orders
</p>
<pre>
s1; s2; </pre>
<p>
or
</p>
<pre>
s2; s1; </pre>
<p>
even if the statements are compound.
</p>
<p>
The <code>foreach</code> statement is a parameterized version of the
<code>each</code> statement. For example,
</p>
<pre>
foreach( i :- 0:n ) {
    a[i].init();
} </pre>
<p>
invokes the <code>init</code> method of <var>n</var> members of array
<code>a</code>.
As for the <code>each</code> statement, it is guaranteed that every
iteration is executed without interference from other iterations.
Thus, an iteration can only influences other iterations
when it has been completed.
</p>
<p>
To allow easier analysis, the <code>foreach</code> has a range syntax rather
than the traditional <code>while</code>-like syntax of the <code>for</code>
statement of Java. For reasons of orthogonality Spar also allows the
range syntax in the <code>for</code> statement, and the <code>while</code>-like
syntax in the <code>foreach</code> statement. For the latter statement
the compiler has the freedom to parallelize it, but early implementations
of the compiler are likely to treat it like a <code>for</code> loop.
</p>
<h2>Vector ranges</h2>
<p>
The <code>foreach</code> range can also be described as a
<a href="vectors.html">vector</a> range.
For example, a two-dimensional array <code>b</code> would be initialized
completely by:
</p>
<pre>
foreach( i :- [0,0]:b.getSize() ){
    (b\@i).init();
} </pre>
<h2>Masked iterations</h2>
<p>
As a further refinement, the range syntax allows <dfn>masks</dfn>. For
example, if we wanted to initialize only the non-null elements of an
array of objects, we could write:
</p>
<pre>
foreach( i :- [0,0]:b.getSize( b ), b\@i != null ) {
    (b\@i).init();
} </pre>
EOF
&generateFooter($0);
