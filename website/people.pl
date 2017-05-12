$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "People working on the Timber compiler";
&generateHeader( $webpage, $title );

print <<EOF;

<h2>Project management</h2>

<p>
<a href="http://www.pds.its.tudelft.nl/~henk/">Henk Sips</a> <br />
<a href="http://www.pds.its.tudelft.nl/~reeuwijk/">Kees van Reeuwijk</a> <br />
<a href="http://rama.pds.twi.tudelft.nl/~gemund/">Arjan J.C. van Gemund</a><br />
</p>

<h2>Sequential compiler</h2>

<p>
<a href="http://www.pds.its.tudelft.nl/~reeuwijk/">Kees van Reeuwijk</a> <br />
Paul Decheringen (Language definition and denotational semantics)<br />
</p>

<h2>Parallelization engines</h2>

<p>
<a href="http://www.pds.its.tudelft.nl/~frits/">Frits Kuijlman</a> <br />
<a href="http://ta.twi.tudelft.nl/wagm/staff/H.X.Lin.html">Hai Xian Lin</a> (Advise on numerical programming)<br />
</p>

<h2>Special projects</h2>

<p>
<a href="http://rama.pds.twi.tudelft.nl/~gemund/">Arjan J.C. van Gemund</a> (Cost estimation)<br />
Andrei Radulescu (Task scheduling) <br />
Leo Breebaart (rule-based compiler generator)<br />
<a href="http://www.pds.its.tudelft.nl/~reeuwijk/">Kees van Reeuwijk</a> (<a href="$tm_url">Template Manager</a>) <br />
</p>

EOF
&generateFooter($0);
