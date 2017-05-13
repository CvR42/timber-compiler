$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - project summary";
&generateHeader( $webpage, $title );
&generate_img( $webpage, "povray/logo_spar.jpg", "" );
print <<EOF;
<h2>Description</h2>
Spar is a programming language for semi-automatic parallel
programming, in particular for the programming of array-based
applications. The language has been designed as part of
the <a href="../automap/index.html">Automap</a> project, in
which a compiler and run-time system are being developed for
distributed-memory systems. As suggested by its name, the goal of <a
href="../automap/index.html">Automap</a> is completely automatic code
and data mapping at either compile-time or run-time. This relieves
the programmer of a difficult task, and makes Spar programs completely
portable.
<p>
Apart from a few minor modifications, Spar is a superset of Java.
This provides Spar with a modern, solid, language as basis, and makes
Spar more accessible.  Spar extends Java with constructs for parallel
programming, extensive support for array manipulation, and a number of
other powerful language extensions.
</p>
<h2>Further information</h2>
For further information, see the following pages:<br>
EOF

&generate_child_selector( $webpage );

print <<EOF;
<h2>People</h2>
<a href="http://www.pds.its.tudelft.nl/~henk/">Henk J. Sips</a> (Project leader) <br>
<a href="http://rama.pds.twi.tudelft.nl/~gemund/">Arjan J.C. van Gemund</a> (Senior researcher, Advise on task mapping and language design)<br>
<a href="http://ta.twi.tudelft.nl/wagm/staff/H.X.Lin.html">Hai Xian Lin</a> (Senior researcher, Advise on numerical programming)<br>
<a href="http://www.pds.its.tudelft.nl/~reeuwijk/">Kees van Reeuwijk</a> (Compiler) <br>
<a href="http://www.pds.its.tudelft.nl/~frits/">Frits Kuijlman</a> (Runtime library) <br>

<h2>Publications</h2>
The Spar language specification has not been formally published, but it is
available for download:
EOF

&generate_downloads( $webpage, "spar/spar-langspec-$spar_langspec_version" );

&generateFooter($0);
