$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar License information";
&generateHeader( $webpage, $title );
print <<EOF;

<p>
The compiler is licenced under the
<a href="http://www.fsf.org/copyleft/gpl.html">GNU Public License</a> (GPL).

<p>
The Spar libraries are covered by the
<a href="http://www.fsf.org/copyleft/lgpl.html">GNU Library Public License</a>.

<p>

Please note that Spar uses the <a href="http://www.kaffe.org/">Kaffe</a>
for its implementation of the Java standard library. Kaffe is also
licenced under the <a href="http://www.fsf.org/copyleft/gpl.html">GNU
Public License</a>.

EOF
&generateFooter($0);
