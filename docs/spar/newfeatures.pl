$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "New features in the Timber compiler";
&generateHeader( $webpage, $title );
$downloading_url = &generate_relative_url( $webpage, "downloading.html" );

print <<EOF;


<h2>New features in Timber 2.0.1 compared to 2.0</h2>
<p>
Timber 2.0.1 was released 13 December 2002.
</p>
<ul>
<li>The elements of an array are no longer stored in a separately allocated block,
but start immediately after the array descriptor.</li>
<li>When a program never catches a null pointer or bounds violation
exception, generate a special handler that does an exit() instead of
throwing an exception, and take this into account in in the program
analysis.</li>
<li>Other optimizations to generated code.</li>
<li>Improved language compatibility.</li>
<li>Added an 'uninstall' make rule.</li>
<li>Added support for MacOS X.</li>
</ul>

<h2>New features in Timber 2.0 compared to 1.6</h2>
<p>
Timber 2.0 was released 28 March 2002.
</p>
<ul>
<li>Compilation of parallel programs is now officially supported. The communication libraries PVM, MPI-lam and MPI-mpich are supported. There are some
limitations; see the README in the distribution.</li>
<li>Further modifications to the compiler to improve language compliance.</li>
</ul>

<h2>New features in Timber 1.6 compared to 1.5</h2>
<p>
Timber 1.6 was released 21 November 2001.
</p>
<ul>
<li>Some modifications to the compiler to improve language compliance.</li>
<li>Some performance improvements.</li>
</ul>

<h2>New features in Timber 1.5 compared to 1.4</h2>
<p>
Timber 1.5 was released 23 October 2001.
</p>

<ul>
<li>A lot of tuning has been done to improve the analyzer, resulting
in better performance for a larger set of programs. For example, an
attempt is made to do static garbage collection, by letting the compiler
insert <code>__delete</code> statements for objects that are known to
be garbage. This currently almost never works for class objects, but
for arrays it works better. In many cases, significantly more efficient
programs are generated.</li>

<li>This version of the compiler uses the new analyzer treewalkers in
Tm version 2.0.8. It may be necessary to get a new version of Tm from
the <a href="$tm_url">Tm website</a>.</li>

<li>The Jacks compiler validation suite has been applied, and a lot
of small details have been modified to be more conformant with Java.
The current score is comparable to that of the Sun Java compiler.</li>

<li>The compiler now contains parallelization engines (in the directory
'engines'). The portability and documentation of these engines does
not yet meet our quality requirements, so for now these engines must
be considered experimental.  We plan to release a working parallelizing
compiler in the near future.</li>
</ul>

<h2>New features in Timber 1.4 compared to 1.3</h2>
<p>
Timber 1.4 was released 22 March 2001.
</p>

<ul>
<li>Inner classes, as described in the Java Language Specification version 2,
are now supported.</li>
<li>The Spar language extensions are now only supported in files ending
with <tt>.spar</tt>. For files ending with <tt>.java</tt> the compiler
behaves as much as possible like an ordinary Java compiler.</li>
<li>Timber 1.4 works with (and requires) version 1.0.6 of the Kaffe runtime library.</li>
<li>Various bug fixes and performance improvements.</li>
</ul>


<h2>New features in Timber 1.3 compared to 1.2</h2>
<p>
Timber 1.3 was released 24 Jan 2001.
</p>

<ul>
<li><p>The compiler is now called the 'timber' compiler, to stress that
there is a difference between the language name Spar/Java and the
compiler that implements it. The command to compile Spar/Java programs
is still 'spar'.</p>
<p> 
(And yes, we're not yet fully consistent in this.)
</p>
</li>
 
<li>The Timber <a href="$downloading_url">downloading page</a>
now also allows subscribing and unsubscribing to the Timber announcements
mailing list with one button click.</li>
 
<li>Vnus now also does analysis and optimizations. In particular, it does
optimization and elemination on null pointer checks and bound checks.</li>
 
<li>Added the <tt>--nogc</tt> and a <tt>--strictanalysis</tt> flags.</li>
 
<li>Added a more efficient nullednewarray construct to Vnus for arrays that
are supposed to be initialized to null.</li>
 
<li>The rank of an array can now also be specified as a compile-time
constant. That is, type declarations like `<tt>int a[*^2]</tt>' are now also
allowed.</li>
 
<li>When new-ing an array, the shape can now also be specified as a
vector expression, by using an explicit '\@'. Thus:
<pre> 
  [int^2] v = [10,20];
  int a[*^2] = new int\@v;</pre>
 
is now supported.
</li>

<li>Spar no longer supports the methods <tt>setSize</tt>,
<tt>setRoom</tt>, and <tt>fitRoom</tt> on arrays.</li>
 
<li><p>Added a <tt>spar.util.Vector</tt> type that is similar to <tt>java.util.Vector</tt>,
but parameterized with the element type of the vector. This replaces
the <tt>setSize</tt> methods on normal arrays for the 1-dimensional case (which
is by far the most common).
</p>
<p> 
This <tt>spar.util.Vector</tt> is used internally to represent the pool of
internalized <tt>String</tt>s, and thereby serves as a (used) example of a
user-defined array representation.                                              
</p>
</li>
</ul>

<h2>New features in Spar 1.2 compared to Spar 1.1</h2>
<p>
Spar 1.2 was released 20 Nov 2000.
</p>

<ul>
<li>The Spar compiler and its documentation now use the second edition
of the Java Language Specification (JLS2 for short) as reference. Inner
interfaces work, but inner classes do not work.</li>
 
<li>Improved checking of programs, including checking of the '<tt>throws</tt>'
clauses of methods.</li>

<li>A lot more agressive analysis of code is done, including
'<tt>final</tt>' analysis of formal parameters and local variables. This
often results in significantly more efficient code.</li>

<li>Inline methods no longer allow type parameters. This feature was
of dubious value, and caused considerable implementation problems. A
workaround is to use generic types, since generic types still support
type parameters.</li>

<li>Preparations have been started to support garbage collection. The
run-time administration that is needed is already generated, and
it is good enough to allow garbage collection on <em>some</em>
programs. Unfortunately it is not good enough to work on all
programs. Nevertheless, the run-time adminstration is there, and should
give a very accurate indication of the eventual garbage-collection
administration overhead.</li>
 
<li>Lots of bug fixes.</li>
</ul>

<h2>New features in Spar 1.1 compared to 1.0.4</h2>

<ul>
<li>Added support for the do-it-yourself infix operator. That is, anywhere
you write <tt>add(a,b)</tt> it is now also allowed to write <tt>a \\add b</tt>.</li>

<li>Spar is now available for downloading from the Spar website.</li>

<li>Tuple widening conversion and tuple narrowing conversion are now
defined and implemented.</li>

<li>Lots of minor changes to make the software more portable.</li>

<li>The compiler now requires at least Tm version 2.0.7 (Use 'tm -h' see
which version you have).</li>


<li>The <tt>macro</tt> keyword has been renamed to <tt>inline</tt>, and
is now officially supported. There are at the moment some restrictions,
though.</li>

<li>Subscript operations on objects are now transformed to calls
to <tt>getElement</tt> or <tt>setElement</tt>. </li>

<li>Fixed a bug that caused incorrect code for a <tt>continue</tt> statement
in classical <tt>for()</tt> loops and <tt>while(){}</tt> loops.</li>

<li><tt>continue</tt> is now also supported for cardinality loops.</li>

<li>Null pointer exceptions are now thrown in all cases.</li>

<li>Class and interface parameters must be surrounded with <tt>(|</tt> and <tt>|)</tt>, both
for formal and actual parameters.</li>

<li>More strict error checking:
<ul>
<li>Local declarations cannot hide other locals or formal parameters.</li>
<li>No <tt>type</tt> formal parameters in normal functions and constructors.</li>

</ul>
</li>
</ul>

<p>
Spar 1.0.4 was an internal release that has never been available for
downloading.
</p>
EOF

&generateFooter($0);
