$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Vnus";
&generateHeader( $webpage, $title );
$timber_download_url = &generate_relative_url( $webpage, "spar/downloading.html" );
$timber_url = &generate_relative_url( $webpage, "index.html" );
$timber_spar_url = &generate_relative_url( $webpage, "spar/index.html" );
print "<p>\n";
&generate_img( $webpage, "povray/logo_vnus.jpg", "" );
print <<EOF;
</p>
<h2>Description</h2>
<p>
Vnus is an intermediate language that is used in the <a
href="$timber_url">Timber</a> compiler environment. Version 1 was originally
designed as intermediate language for the Booster programming language,
but version 2 has been modified to support <a href="$timber_spar_url">Spar</a>.
</p>
<p>
For version 1 of the language a denotational semantics has been developed.
</p>
<p>
For version 2 a translator is available that generates C with a few C++
constructs (mainly exceptions).
</p>
<p>
Many of the transformations and optimizations of the compiler are
implemented as transformations on Vnus.
</p>

<h2>Publications</h2>
<p>
The Vnus reference manual has not been formally published, but it is available
for download:
</p>
EOF

&generate_downloads( $webpage, "vnus/vnus-reference" );

print <<EOF;
<p>
Further publications related to Vnus <a
href="$publication_search_url?title=yes&query=vnus">are listed
in the PDS publication database</a>.
</p>
<h2>Availability</h2>
<p>
The Vnus to C++ converter can be <a href="$timber_download_url">downloaded</a>
as part of the timber compiler distribution.
</p>
EOF
&generateFooter($0);
