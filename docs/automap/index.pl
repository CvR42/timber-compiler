$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Automap - project summary";
&generateHeader( $webpage, $title );

&generate_img( $webpage, "povray/logo_automap.jpg", "" );

$spar_link = &generate_relative_url( $webpage, "spar/index.html" );
$timber_link = &generate_relative_url( $webpage, "index.html" );
print <<EOF;

<h2>Description</h2>
The goal of the Automap project is to develop a compiler for
<em>high-performance</em> <em>scientific</em> computations.  To realize
this goal, two important components must be developed: a programming
language in which it is convenient to express scientific programs,
and a framework that allows programs to be <em>parallelized</em> easily.

<p>
The first component, a suitable programming language, is being developed
as <a href="$spar_link">Spar</a>, a super-set of Java.
<p>
The second component, a framework for convenient parallelization, is being
developed as the Hazel parallelization engine.

<h2>Relations with other projects</h2>
Automap provides a significant contribution to the <a
href="$timber_link">Timber</a> project.

EOF
&generateFooter($0);
