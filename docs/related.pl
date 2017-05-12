$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Related work";
&generateHeader( $webpage, $title );

print <<EOF;
<h2>Java language</h2>

<p>
|
<a href="http://java.sun.com/">JavaSoft</a> |
<a href="http://www.ergnosis.com/java-spec-report/">Java Spec. report</a> |
<a href="http://oss.software.ibm.com/developerworks/opensource/jacks/">Jacks Java language testsuite</a> |
<a href="http://sources.redhat.com/mauve/">Mauve Java library testsuite</a> |
<a href="http://www.ipd.ira.uka.de/~pizza/">Pizza</a> |
<a href="http://www.jddarcy.org/Borneo/">Borneo</a> |
</p>

<h2>Java implementation</h2>

<p>
|
<a href="http://java.sun.com/products/jdk/1.1/docs/guide/jni/spec/jniTOC.doc.html">JNI</a> |
<a href="http://gcc.gnu.org/java/papers/native++.html">KNI</a> |
<a href="http://stork.ukc.ac.uk/computer_science/Html/Jones/GC/gcbook.html">Garbage Collection</a> |
</p>

<h2>Affiliated projects</h2>

<p>
The <a href="$joses_url">Joses</a> project.
</p>
EOF


&generate_child_selector($webpage);

print <<EOF;

<h2>Benchmarks</h2>

<p>
|
<a href="http://www.epcc.ed.ac.uk/research/javagrande/">Javagrande Benchmark suite</a> |
</p>

<h2>Windowing</h2>

<p>
|
<a href="http://www.ii.uib.no/~rolfwr/">JCNIX</a> |
<a href="http://www.cs.umb.edu/~eugene/XTC/">the X Tool Collection</a> |
</p>

<h2>Parallel and high-performance Java</h2>
<ul>
<li><a href="http://www.cs.berkeley.edu/Research/Projects/titanium/">Titanium</a></li>
<li><a href="http://java.sun.com/people/jag/FP.html">James Gosling: The Evolution of Numerical Computing in Java</a></li>
</ul>

<h2>Other Java language extensions</h2>
<ul>
<li><a href="http://www.javaworld.com/javaworld/javaone98/j1-98-interview.gosling.html">James Gosling about language extensions</a></li>
</ul>

<h2>Further links</h2>

<ul>
<li><a href="http://www.cs.ucsb.edu/conferences/java98/">ACM 1998 Workshop on Java for High-Performance Network Computing</a></li>
<li><a href="http://grunge.cs.tu-berlin.de/~tolk/vmlanguages.html">Programming languages for the JVM</a></li>
<li><a href="http://www.developer.com/java/">Gamelan</a></li>
<li><a href="http://sunsite.unc.edu/javafaq/javafaq.html">comp.lang.java FAQ</a></li>
</ul>
EOF

&generateFooter($0);
