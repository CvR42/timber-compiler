$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Implementation of the Timber compiler";
&generateHeader( $webpage, $title );

&generate_under_construction( $webpage );

print <<EOF;
<h2>Implementation</h2>
<p>
The Timber compiler consists of the following components:
</p>
<ul>
<li><a href="spar/index.html">Spar</a>, a set of language extensions to Java, and a frontend to Vnus that implements this language.</li>
<li><a href="vnus/index.html">Vnus</a>: an intermediate language, currently supported by a backend that generates C++.</li>
</ul>
<p>
These components are supported by two of code-generation tools:
</p>
<ul>
<li><a href="rotan/index.html">Rotan</a>: a rule-based transformation system.</li>
<li><a href="$tm_url">Tm</a>: a macro language for the generation of
tree manipulation code.</li>
</ul>
<p>
Tm is used extensively in the current implementation. For example,
it is estimated that 30% of the code of the Spar frontend is generated
by Tm.
</p>

EOF

&generate_child_selector( $webpage );

&generateFooter($0);
