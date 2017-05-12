$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar missing features";
&generateHeader( $webpage, $title );
print <<EOF;
The most important missing features are:
<ul>

<li>Optimization. Only a limited amount of optimization is currently
implemented. This means that some language features (e.g. tuples)
are slower than we would like them to be.</li>

<li>Further Java compatability. Since Spar is a batch compiler from
Java source code to native code, it has some inherent difficulty in
implementing some Java features (e.g. dynamic class loading).</li>

</ul>
EOF
&generateFooter($0);
