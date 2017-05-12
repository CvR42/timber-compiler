$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "The Timber compiler";
&generateHeader( $webpage, $title );
$downloading_url = &generate_relative_url( $webpage, "downloading.html" );
$news_url = &generate_relative_url( $webpage, "news.html" );
$information_url = &generate_relative_url( $webpage, "documents.html" );
$implementation_url = &generate_relative_url( $webpage, "implementation.html" );
$people_url = &generate_relative_url( $webpage, "people.html" );
$spar_overview_url = &generate_relative_url( $webpage, "spar/overview.html" );
$links_url = &generate_relative_url( $webpage, "related.html" );
$jacks_url = &generate_relative_url( $webpage, "jacks.html" );

print "<p>\n";

&generate_img( $webpage, "povray/logo_timber.jpg", "" );
print <<EOF;
</p>

<h2>Description</h2>
<p>
The Timber compiler is a parallelizing static compiler for a superset of Java.
</p>
<p>
The Timber compiler has been developed to demonstrate, and experiment
with, compilation techniques for parallel and distributed systems.
Our research concentrates on annotation-based parallel programming,
so that is what the compiler supports. To provide a realistic context
for parallelization engines, sufficient optimizations for sequential
programs have been implemented to achieve a respectable level of
performance for sequential programs.
</p>
<p>
The compiler implements most of Java, plus a set of additional language
constructs for scientific programming. This set of extensions is called
<em>Spar</em>. We only fundamentally deviate from the Java language
specification on the following points:
</p>
<ul>
<li>The compiler is a whole-program static compiler from Java/Spar source code
to machine code, so JVM bytecode is not supported. In particular, dynamic
class loading is not supported.</li>
<li>Java threads are not supported. Instead, we provide our own
language constructs for parallel programming.</li>
<li>The <a href="$spar_overview_url">Spar language extensions</a> introduce a number of additional keywords.
</li>
</ul>
The additional language constructs are only recognized in files with
the extension <code>.spar</code>. For code in <code>.java</code> and
<code>.jav</code> files, the compiler behaves pretty much like an ordinary
Java compiler. In fact, for the <a href="$jacks_url">Jacks validation
suite</a>, we have pretty much the same score as Sun's own compiler!


<h2>Downloading</h2>
<p>
The compiler is available for downloading under the Gnu Public License
(GPL). See the <a href="$downloading_url">downloading</a> page for
further details.
</p>

<h2>Further information</h2>
<p>
There are separate pages for 
<a href="$news_url">Timber News</a>;
<a href="$information_url">Publications</a>;
<a href="$people_url">people</a>;
an <a href="$spar_overview_url">overview of Spar</a>;
<a href="$implementation_url">implementation</a> 
and <a href="$jacks_url">language compliance</a> of the Timber compiler</a>;
and <a href="$links_url">related websites</a>.
</p>

<h2>Affiliation</h2>
<p>
The Timber compiler is a long-term project of the <a
href="http://www.pds.its.tudelft.nl/">Parallel and Distributed Systems</a>
group of <a href="http://www.tudelft.nl/">Delft University of
Technology</a>.
</p>

EOF

&generateFooter($0);
