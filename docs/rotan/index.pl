$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Rotan";
&generateHeader( $webpage, $title );
$timber_url = &generate_relative_url( $webpage, "index.html" );
&generate_img( $webpage, "povray/logo_rotan.jpg", "" );
print <<EOF;

<h2>Description</h2>
Rotan is a rule-base code transformation system. It is part of the
<a href="$timber_url">Timber</a> project.
<h2>People</h2>
<a href="http://www.pds.its.tudelft.nl/~henk/">Henk J. Sips</a> (Project leader) <br>
Leo Breebaart (Design and implementation)<br>
<a href="http://www.pds.its.tudelft.nl/~frits/">Frits Kuijlman</a> (Maintainer)<br>

EOF
&generateFooter($0);
